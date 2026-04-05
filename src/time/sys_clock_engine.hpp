#pragma once

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <functional>
#include <iostream>
#include <mutex>
#include <optional>
#include <queue>
#include <thread>
#include <variant>
#include <vector>

#ifdef __linux__
#include <pthread.h>
#elif defined(__APPLE__)
#include <pthread.h>
#elif defined(_WIN32)
#include <windows.h>
#endif

namespace idyl::time {

// ---------------------------------------------------------------------------
// Clock abstraction
// Selects the highest-resolution steady clock available on the platform.
// ---------------------------------------------------------------------------

using steady_clock_t = std::chrono::steady_clock;
using time_point_t   = steady_clock_t::time_point;
using duration_t     = steady_clock_t::duration;

// ---------------------------------------------------------------------------
// scheduled_event
//
// A generic, self-rescheduling event.
//   - member_time     : absolute time at which the event should fire
//   - member_callback : callable executed by the runner thread
//                       returns an optional<time_point_t>:
//                         * nullopt  → one-shot, do not reschedule
//                         * value    → reschedule at the returned time
//
// Template parameter T lets users attach arbitrary payload.
// ---------------------------------------------------------------------------

template <typename T = std::monostate>
struct scheduled_event {
    using callback_t = std::function<std::optional<time_point_t>(T&)>;

    time_point_t member_time;
    callback_t   member_callback;
    T            member_payload;

    // Comparison for the min-heap (earliest time = highest priority)
    bool operator>(const scheduled_event& other) const noexcept {
        return member_time > other.member_time;
    }
};

// ---------------------------------------------------------------------------
// scheduler
//
// Owns a dedicated timing thread that sleeps until the next event is due,
// then hands events off to a runner callable (e.g. a thread-pool enqueue).
//
// Template parameter T is forwarded to scheduled_event<T>.
//
// Design notes
// ────────────
//  • Lazy + busy wait hybrid
//      The thread sleeps (condition_variable::wait_until) until
//      (next_event_time - busy_window_), then spins on the clock until the
//      exact fire time is reached.  busy_window_ is configurable.
//
//  • Drift prevention
//      Fire times are absolute time_point values.  Rescheduled events are
//      given their *intended* next time (computed from the previous deadline,
//      not from "now"), so accumulated error stays bounded.
//
//  • Thread safety
//      add_event() is safe to call from any thread at any time.
// ---------------------------------------------------------------------------

template <typename T = std::monostate>
struct scheduler {
    using event_t  = scheduled_event<T>;
    using runner_t = std::function<void(event_t)>;

    // -----------------------------------------------------------------------
    // Construction / destruction
    // -----------------------------------------------------------------------

    // runner   : called by the scheduler thread whenever an event fires.
    //            Must be thread-safe (e.g. enqueue to a thread pool).
    // busy_window : how far ahead of the deadline to exit lazy sleep and
    //               start busy-waiting. Default: 2 ms — tune per platform.
    explicit scheduler(runner_t runner,
                       duration_t busy_window = std::chrono::milliseconds(2))
        : _runner(std::move(runner))
        , _busy_window(busy_window)
        , _running(false)
    {}

    ~scheduler() { stop(); }

    // Non-copyable, non-movable (owns a thread)
    scheduler(const scheduler&)            = delete;
    scheduler& operator=(const scheduler&) = delete;
    scheduler(scheduler&&)                 = delete;
    scheduler& operator=(scheduler&&)      = delete;

    // -----------------------------------------------------------------------
    // Lifecycle
    // -----------------------------------------------------------------------

    void start() {
        bool expected = false;
        if (!_running.compare_exchange_strong(expected, true))
            return; // already running

        _thread = std::thread(&scheduler::_loop, this);

        if(_thread.joinable() && high_priority_scheduler) {
            #ifdef __linux__
            // Example: set to SCHED_FIFO with high priority (requires root)
            struct sched_param param;
            param.sched_priority = sched_get_priority_max(SCHED_FIFO);
            if (pthread_setschedparam(_thread.native_handle(), SCHED_FIFO, &param) != 0) {
                std::cerr << "Warning: Failed to set scheduler thread priority\n";
            }
            #elif defined(__APPLE__)
            // macOS does not support SCHED_FIFO, but we can set a QoS class
            pthread_set_qos_class_self_np(QOS_CLASS_USER_INTERACTIVE, 0);
            #elif defined(_WIN32)
            // Windows thread priority adjustment (requires appropriate privileges)
            HANDLE thread_handle = _thread.native_handle();
            if (!SetThreadPriority(thread_handle, THREAD_PRIORITY_HIGHEST)) {
                std::cerr << "Warning: Failed to set scheduler thread priority\n";
            }
            #endif
        }
    }

    void stop() {
        bool expected = true;
        if (!_running.compare_exchange_strong(expected, false))
            return; // already stopped

        _cv.notify_all();
        if (_thread.joinable())
            _thread.join();
    }

    // -----------------------------------------------------------------------
    // Public API
    // -----------------------------------------------------------------------

    // Add an event from any thread.
    // Thread-safe — wakes the scheduler thread immediately so it can
    // re-evaluate the queue head.
    void add_event(event_t ev) {
        {
            std::lock_guard<std::mutex> lock(_mutex);
            _queue.push(std::move(ev));
        }
        _cv.notify_one();
    }

    // Convenience overload: build the event inline.
    void add_event(time_point_t when,
                   typename event_t::callback_t callback,
                   T payload = T{}) {
        add_event(event_t{when, std::move(callback), std::move(payload)});
    }

    // -----------------------------------------------------------------------
    // Accessors
    // -----------------------------------------------------------------------

    bool          is_running()    const noexcept { return _running.load(); }
    std::size_t   pending_count() const noexcept {
        std::lock_guard<std::mutex> lock(_mutex);
        return _queue.size();
    }

    // Adjust the busy-wait window at runtime (e.g. after profiling).
    void set_busy_window(duration_t d) noexcept { _busy_window = d; }

private:
    // -----------------------------------------------------------------------
    // Internal types
    // -----------------------------------------------------------------------

    using heap_t = std::priority_queue<event_t,
                                       std::vector<event_t>,
                                       std::greater<event_t>>;

    // -----------------------------------------------------------------------
    // Scheduler thread loop
    // -----------------------------------------------------------------------

    void _loop() {
        while (_running.load(std::memory_order_acquire)) {
            std::unique_lock<std::mutex> lock(_mutex);

            // ── Lazy sleep until we have an event ──────────────────────────
            _cv.wait(lock, [this] {
                return !_queue.empty() || !_running.load(std::memory_order_relaxed);
            });

            if (!_running.load(std::memory_order_relaxed))
                break;

            // Peek at the nearest event
            time_point_t deadline = _queue.top().member_time;

            // ── Lazy sleep until (deadline - busy_window) ──────────────────
            // We release the lock so add_event() can insert while we sleep.
            auto wake_at = deadline - _busy_window;
            _cv.wait_until(lock, wake_at, [this, &deadline] {
                // Wake early if a closer event was added
                return !_running.load(std::memory_order_relaxed)
                    || (!_queue.empty()
                        && _queue.top().member_time < deadline);
            });

            if (!_running.load(std::memory_order_relaxed))
                break;

            // Update deadline in case a closer event arrived
            deadline = _queue.top().member_time;
            lock.unlock();

            // ── Busy-wait until exact deadline ─────────────────────────────
            while (steady_clock_t::now() < deadline) {
                // Yield to avoid starving other threads on the same core,
                // while still burning cycles for accuracy.
                std::this_thread::yield();
            }

            // ── Fire all events whose deadline has passed ──────────────────
            lock.lock();
            while (!_queue.empty() && _queue.top().member_time <= steady_clock_t::now()) {
                // Pop by value (priority_queue gives const ref to top)
                event_t ev = _queue.top();
                _queue.pop();
                lock.unlock();

                // Hand off to runner (non-blocking from scheduler's POV)
                _fire(std::move(ev));

                lock.lock();
            }
        }
    }

    // -----------------------------------------------------------------------
    // Fire one event, handle optional rescheduling
    // -----------------------------------------------------------------------

    void _fire(event_t ev) {
        // Capture the intended deadline before moving the event
        time_point_t intended_time = ev.member_time;

        // Invoke runner — runner owns the event from this point
        // We pass a copy so we can inspect the callback's return value.
        // If rescheduling is needed the runner must call add_event() itself,
        // OR we can use a wrapper approach (see below).
        //
        // Rescheduling wrapper: the runner receives the event; after executing
        // the user callback it checks the return value and calls add_event()
        // on this scheduler.  To keep scheduler generic, we bake the
        // rescheduling logic here and let _runner just execute the payload.
        _runner_wrapper(std::move(ev), intended_time);
    }

    // Wraps the user runner to handle rescheduling transparently.
    void _runner_wrapper(event_t ev, time_point_t /*intended_time*/) {
        // We need the callback result after the runner executes.
        // Strategy: run the callback directly here (on the scheduler thread)
        // only to get the reschedule time, then dispatch work to _runner.
        //
        // For heavier workloads, the user's runner should be async
        // (e.g. thread pool). The callback itself should be cheap — it
        // should only decide *when* to reschedule and enqueue real work.

        auto next = ev.member_callback(ev.member_payload);

        // Dispatch to runner thread (fire-and-forget, non-blocking)
        _runner(ev);  // runner receives the event (with updated payload if any)

        // Reschedule with drift-free next time
        if (next.has_value()) {
            ev.member_time = *next;
            add_event(std::move(ev));
        }
    }

    // -----------------------------------------------------------------------
    // Members
    // -----------------------------------------------------------------------

    runner_t              _runner;
    duration_t            _busy_window;
    std::atomic<bool>     _running;

    mutable std::mutex    _mutex;
    std::condition_variable _cv;
    heap_t                _queue;

    std::thread           _thread;

    bool high_priority_scheduler = false; 
};

} // namespace idyl


// =============================================================================
// PLATFORM NOTES — Thread priority
// =============================================================================
//
// For maximum accuracy, elevate the scheduler thread priority after start().
// Example (Linux / POSIX):
//
//   #include <pthread.h>
//   sched_param sp{ .sched_priority = sched_get_priority_max(SCHED_FIFO) };
//   pthread_setschedparam(_thread.native_handle(), SCHED_FIFO, &sp);
//
// Example (Windows):
//
//   SetThreadPriority(_thread.native_handle(), THREAD_PRIORITY_TIME_CRITICAL);
//
// Example (macOS):
//
//   pthread_set_qos_class_self_np(QOS_CLASS_USER_INTERACTIVE, 0);
//
// These require elevated privileges and should be opt-in.
// =============================================================================