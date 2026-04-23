#pragma once

// =============================================================================
// audio_clock_scheduler.hpp — high-resolution timer scheduler (cross-platform)
//
// Fires at "buffer rate" (buffer_size / sample_rate Hz) using the best
// OS timer available on the current platform:
//
//   Linux   : timerfd_create(CLOCK_MONOTONIC)   kernel-precision periodic wake
//   macOS   : mach_wait_until                   same primitive as Core Audio
//   Windows : CreateWaitableTimerEx (HRTIMER)   Win10 1803+ ≈ 0.5 ms accuracy
//             → timeBeginPeriod(1) fallback      older Windows ≈ 1 ms
//   other   : std::this_thread::sleep_until      ≈ 1–5 ms OS jitter
//
// No audio device is ever opened, so this never conflicts with Csound,
// Jack, PipeWire, or any other audio engine running in the same process.
//
// Default period: 32 frames / 48 kHz ≈ 0.67 ms
//
// Architecture
// ────────────
//   timer thread → wakes at each period → checks all subscriptions → fires due
//
// Drift-free scheduling:
//   next_fire_ms = prev_fire_ms + dt_ms
// =============================================================================

#ifdef IDYL_AUDIO_CLOCK

#include "time/scheduler.hpp"

#include <atomic>
#include <chrono>
#include <iostream>
#include <mutex>
#include <thread>
#include <unordered_map>
#include <vector>

// ── Platform headers ──────────────────────────────────────────────────────────
#if defined(__linux__)
#  include <poll.h>
#  include <sys/timerfd.h>
#  include <unistd.h>
#elif defined(__APPLE__)
#  include <mach/mach_time.h>
#elif defined(_WIN32)
#  ifndef WIN32_LEAN_AND_MEAN
#    define WIN32_LEAN_AND_MEAN
#  endif
#  include <windows.h>
#  include <timeapi.h>
   // Added in Win10 1803; define locally so we compile against older SDKs too.
#  ifndef CREATE_WAITABLE_TIMER_HIGH_RESOLUTION
#    define CREATE_WAITABLE_TIMER_HIGH_RESOLUTION 0x00000002
#  endif
#endif

namespace idyl::time {

// ════════════════════════════════════════════════════════════════════════════════
// audio_clock_scheduler
// ════════════════════════════════════════════════════════════════════════════════

struct audio_clock_scheduler : idyl_scheduler {

    // ── Configuration ──────────────────────────────────────────────────────────
    unsigned int sample_rate_ = 48000;
    unsigned int buffer_size_ = 32;

    // ── Construction ───────────────────────────────────────────────────────────

    audio_clock_scheduler() = default;

    explicit audio_clock_scheduler(unsigned int sample_rate,
                                   unsigned int buffer_size)
        : sample_rate_(sample_rate), buffer_size_(buffer_size)
    {}

    ~audio_clock_scheduler() noexcept override { stop(); }

    audio_clock_scheduler(const audio_clock_scheduler&)            = delete;
    audio_clock_scheduler& operator=(const audio_clock_scheduler&) = delete;
    audio_clock_scheduler(audio_clock_scheduler&&)                 = delete;
    audio_clock_scheduler& operator=(audio_clock_scheduler&&)      = delete;

    // ── Lifecycle ──────────────────────────────────────────────────────────────

    void start() override {
        if (running_.load()) return;

        period_ns_ = static_cast<long long>(
            static_cast<double>(buffer_size_) / static_cast<double>(sample_rate_) * 1.0e9);

        origin_  = steady_clock_t::now();
        running_.store(true);
        worker_  = std::thread([this] { worker_loop_(); });

        std::cerr << "[audio_clock] running at " << sample_rate_ << " Hz, "
                  << buffer_size_ << " frames/tick ("
                  << static_cast<double>(period_ns_) / 1.0e6 << " ms/tick)\n";
    }

    void stop() override {
        if (!running_.exchange(false)) return;
        if (worker_.joinable()) worker_.join();
        std::lock_guard<std::mutex> lk(subs_mutex_);
        subscriptions_.clear();
    }

    bool is_running() const noexcept override { return running_.load(); }

    // ── Subscribe / unsubscribe ────────────────────────────────────────────────

    subscription_id subscribe(double dt_ms, tick_fn callback) override {
        subscription_id id = next_id_++;

        subs_entry e;
        e.id_           = id;
        e.dt_ms_        = dt_ms;
        e.callback_     = std::move(callback);
        e.next_fire_ms_ = now_ms() + dt_ms;
        e.active_       = true;

        std::lock_guard<std::mutex> lk(subs_mutex_);
        subscriptions_[id] = std::move(e);
        return id;
    }

    void unsubscribe(subscription_id id) override {
        std::lock_guard<std::mutex> lk(subs_mutex_);
        auto it = subscriptions_.find(id);
        if (it != subscriptions_.end())
            it->second.active_ = false;
    }

    void update_dt(subscription_id id, double new_dt_ms) override {
        std::lock_guard<std::mutex> lk(subs_mutex_);
        auto it = subscriptions_.find(id);
        if (it != subscriptions_.end())
            it->second.dt_ms_ = new_dt_ms;
    }

    // ── Queries ────────────────────────────────────────────────────────────────

    double now_ms() const noexcept override {
        auto elapsed = steady_clock_t::now() - origin_;
        return std::chrono::duration<double, std::milli>(elapsed).count();
    }

    std::size_t active_count() const noexcept override {
        std::lock_guard<std::mutex> lk(subs_mutex_);
        std::size_t n = 0;
        for (const auto& [_, s] : subscriptions_)
            if (s.active_) ++n;
        return n;
    }

private:

    // ── Internal subscription record ───────────────────────────────────────────
    struct subs_entry {
        subscription_id id_           = 0;
        double          dt_ms_        = 0.0;
        double          next_fire_ms_ = 0.0;
        tick_fn         callback_;
        bool            active_       = true;
    };

    // ── State ──────────────────────────────────────────────────────────────────
    long long      period_ns_ = 0;
    time_point_t   origin_;

    std::atomic<bool>  running_{false};
    std::thread        worker_;

    mutable std::mutex                               subs_mutex_;
    std::unordered_map<subscription_id, subs_entry>  subscriptions_;
    std::atomic<subscription_id>                     next_id_{1};

    // ── Worker thread — platform dispatch ─────────────────────────────────────

    void worker_loop_() {
// ─── Linux ────────────────────────────────────────────────────────────────────
#if defined(__linux__)
        int fd = timerfd_create(CLOCK_MONOTONIC, 0);
        if (fd >= 0) {
            struct itimerspec its{};
            its.it_value.tv_sec  = period_ns_ / 1'000'000'000LL;
            its.it_value.tv_nsec = period_ns_ % 1'000'000'000LL;
            its.it_interval      = its.it_value;
            timerfd_settime(fd, 0, &its, nullptr);

            while (running_.load(std::memory_order_relaxed)) {
                // poll with 5ms timeout → stop() latency ≤ 5ms
                struct pollfd pfd{ fd, POLLIN, 0 };
                if (poll(&pfd, 1, 5) <= 0) continue;
                uint64_t exp = 0;
                if (read(fd, &exp, sizeof(exp)) != sizeof(exp)) break;
                if (!running_.load(std::memory_order_relaxed)) break;
                fire_due_();
            }
            close(fd);
            return;
        }
        std::cerr << "[audio_clock] timerfd unavailable, using sleep fallback\n";

// ─── macOS ────────────────────────────────────────────────────────────────────
#elif defined(__APPLE__)
        // mach_wait_until: hard-waits until the Mach absolute time reaches the
        // target. Sub-100µs jitter — the same primitive Core Audio uses internally.
        // No audio device is opened.
        mach_timebase_info_data_t info{};
        mach_timebase_info(&info);
        // Convert period_ns_ to Mach absolute time ticks:
        //   ticks = nanoseconds * denom / numer
        const uint64_t period_ticks =
            static_cast<uint64_t>(period_ns_) * info.denom / info.numer;

        uint64_t next = mach_absolute_time() + period_ticks;
        while (running_.load(std::memory_order_relaxed)) {
            mach_wait_until(next);
            next += period_ticks;
            if (!running_.load(std::memory_order_relaxed)) break;
            fire_due_();
        }
        return;

// ─── Windows ─────────────────────────────────────────────────────────────────
#elif defined(_WIN32)
        // Try high-resolution waitable timer (Windows 10 1803+).
        // Falls back to regular waitable timer + timeBeginPeriod(1) on older systems.
        HANDLE timer = CreateWaitableTimerExW(nullptr, nullptr,
                            CREATE_WAITABLE_TIMER_HIGH_RESOLUTION, TIMER_ALL_ACCESS);
        bool need_timeend = false;
        if (!timer) {
            timer = CreateWaitableTimerW(nullptr, FALSE, nullptr);
            if (timer) {
                timeBeginPeriod(1);
                need_timeend = true;
            }
        }
        if (timer) {
            // Due time: negative value = relative, unit = 100 ns
            LARGE_INTEGER due;
            due.QuadPart = -(static_cast<LONGLONG>(period_ns_) / 100LL);
            // Periodic interval in ms (SetWaitableTimer period is LONG in ms)
            LONG period_ms = static_cast<LONG>(
                std::max(1LL, static_cast<long long>(period_ns_) / 1'000'000LL));
            SetWaitableTimer(timer, &due, period_ms, nullptr, nullptr, FALSE);

            while (running_.load(std::memory_order_relaxed)) {
                // 10ms timeout keeps stop() responsive even without a wakeup signal
                if (WaitForSingleObject(timer, 10) == WAIT_OBJECT_0) {
                    if (running_.load(std::memory_order_relaxed)) fire_due_();
                }
            }
            CancelWaitableTimer(timer);
            CloseHandle(timer);
            if (need_timeend) timeEndPeriod(1);
            return;
        }
        std::cerr << "[audio_clock] waitable timer unavailable, using sleep fallback\n";
#endif

// ─── Portable fallback ────────────────────────────────────────────────────────
        // sleep_until: typically ~1–5ms jitter depending on OS scheduler.
        auto next_tick = steady_clock_t::now() + std::chrono::nanoseconds(period_ns_);
        while (running_.load(std::memory_order_relaxed)) {
            std::this_thread::sleep_until(next_tick);
            next_tick += std::chrono::nanoseconds(period_ns_);
            if (!running_.load(std::memory_order_relaxed)) break;
            fire_due_();
        }
    }

    // ── Fire all subscriptions that are due ────────────────────────────────────
    void fire_due_() {
        const double now = now_ms();

        std::vector<subscription_id> due;
        {
            std::lock_guard<std::mutex> lk(subs_mutex_);
            for (const auto& [id, e] : subscriptions_)
                if (e.active_ && now >= e.next_fire_ms_)
                    due.push_back(id);
        }

        for (subscription_id id : due) {
            tick_fn cb;
            double  dt_ms        = 0.0;
            double  prev_fire_ms = 0.0;

            {
                std::lock_guard<std::mutex> lk(subs_mutex_);
                auto it = subscriptions_.find(id);
                if (it == subscriptions_.end() || !it->second.active_) continue;
                cb           = it->second.callback_;
                dt_ms        = it->second.dt_ms_;
                prev_fire_ms = it->second.next_fire_ms_;
            }

            const bool keep = cb(now, dt_ms);

            {
                std::lock_guard<std::mutex> lk(subs_mutex_);
                auto it = subscriptions_.find(id);
                if (it == subscriptions_.end()) continue;

                if (!keep) {
                    subscriptions_.erase(it);
                } else {
                    // Drift-free: advance from scheduled deadline.
                    // Catch up by whole periods if we fell behind.
                    double next = prev_fire_ms + it->second.dt_ms_;
                    while (next <= now)
                        next += it->second.dt_ms_;
                    it->second.next_fire_ms_ = next;
                }
            }
        }
    }
};

} // namespace idyl::time

#endif // IDYL_AUDIO_CLOCK
