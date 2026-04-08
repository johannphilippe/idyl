#pragma once

// ════════════════════════════════════════════════════════════════════════════════
// scheduler.hpp — Idyl time scheduler
//
// Domain-specific wrapper that connects a timing engine (sys_clock, audio
// callback, etc.) to idyl's temporal function update model.
//
// The scheduler owns subscriptions: each represents a temporal function
// instance that should be called back at regular intervals (its `dt`).
// The underlying engine handles the actual timing; this layer handles
// the idyl-specific bookkeeping (subscriptions, triggers, global time).
//
// Usage:
//     idyl::time::sys_clock_scheduler sched;
//     sched.start();
//     auto id = sched.subscribe(10.0, [](double t, double dt) { ... });
//     ...
//     sched.unsubscribe(id);
//     sched.stop();
// ════════════════════════════════════════════════════════════════════════════════

#include <cstdint>
#include <functional>
#include <mutex>
#include <string>
#include <unordered_map>
#include <vector>

#include "time/sys_clock_engine.hpp"

namespace idyl::time {

// ── Subscription handle ────────────────────────────────────────────────────────
// Returned by subscribe(); used to unsubscribe later.
using subscription_id = uint64_t;

// ── Tick callback signature ────────────────────────────────────────────────────
// Called by the scheduler each time a subscription fires.
//   current_time_ms : absolute wall-clock time since scheduler start (ms)
//   dt_ms           : the subscription's tick interval (ms)
// Returns true to keep the subscription alive, false to auto-unsubscribe.
using tick_fn = std::function<bool(double current_time_ms, double dt_ms)>;

// ── Subscription record ────────────────────────────────────────────────────────
struct subscription {
    subscription_id  id_       = 0;
    double           dt_ms_    = 0.0;     // tick interval in milliseconds
    tick_fn          callback_;
    bool             active_   = true;
};

// ════════════════════════════════════════════════════════════════════════════════
// sys_clock_scheduler
//
// Wraps sys_clock_engine::scheduler to drive idyl temporal function updates
// from the system's steady clock.
//
// A second engine type (e.g. audio_callback_scheduler) can follow the same
// public API shape, or both can be placed behind a variant / type-erased
// interface when the need arises.
// ════════════════════════════════════════════════════════════════════════════════

struct sys_clock_scheduler {

    // ── Construction ───────────────────────────────────────────────────────────

    explicit sys_clock_scheduler(
        duration_t busy_window = std::chrono::microseconds(500))
        : engine_([this](auto ev) { on_engine_event_(ev); }, busy_window)
    {}

    ~sys_clock_scheduler() { stop(); }

    // Non-copyable, non-movable (owns engine which owns a thread)
    sys_clock_scheduler(const sys_clock_scheduler&)            = delete;
    sys_clock_scheduler& operator=(const sys_clock_scheduler&) = delete;
    sys_clock_scheduler(sys_clock_scheduler&&)                 = delete;
    sys_clock_scheduler& operator=(sys_clock_scheduler&&)      = delete;

    // ── Lifecycle ──────────────────────────────────────────────────────────────

    void start() {
        origin_ = steady_clock_t::now();
        engine_.start();
    }

    void stop() {
        engine_.stop();

        std::lock_guard<std::mutex> lock(subs_mutex_);
        subscriptions_.clear();
    }

    bool is_running() const noexcept { return engine_.is_running(); }

    // ── Subscribe / unsubscribe ────────────────────────────────────────────────

    // Register a periodic callback with the given tick interval.
    // Returns a handle that can be passed to unsubscribe().
    subscription_id subscribe(double dt_ms, tick_fn callback) {
        subscription_id id = next_id_++;

        subscription sub;
        sub.id_       = id;
        sub.dt_ms_    = dt_ms;
        sub.callback_ = std::move(callback);
        sub.active_   = true;

        {
            std::lock_guard<std::mutex> lock(subs_mutex_);
            subscriptions_[id] = std::move(sub);
        }

        // Schedule the first tick
        schedule_next_(id, steady_clock_t::now());
        return id;
    }

    // Remove a subscription. Safe to call from any thread, including
    // from inside a tick callback (deferred removal).
    void unsubscribe(subscription_id id) {
        std::lock_guard<std::mutex> lock(subs_mutex_);
        auto it = subscriptions_.find(id);
        if (it != subscriptions_.end()) {
            it->second.active_ = false;   // engine will skip & drop
        }
    }

    // Update the tick interval of an existing subscription.
    // Takes effect on the next reschedule (after the current tick completes).
    void update_dt(subscription_id id, double new_dt_ms) {
        std::lock_guard<std::mutex> lock(subs_mutex_);
        auto it = subscriptions_.find(id);
        if (it != subscriptions_.end())
            it->second.dt_ms_ = new_dt_ms;
    }

    // ── Queries ────────────────────────────────────────────────────────────────

    // Elapsed time since start() in milliseconds.
    double now_ms() const noexcept {
        auto elapsed = steady_clock_t::now() - origin_;
        return std::chrono::duration<double, std::milli>(elapsed).count();
    }

    std::size_t active_count() const noexcept {
        std::lock_guard<std::mutex> lock(subs_mutex_);
        std::size_t n = 0;
        for (const auto& [_, s] : subscriptions_)
            if (s.active_) ++n;
        return n;
    }

private:

    // ── Payload carried through the engine's event queue ───────────────────────
    struct tick_payload {
        subscription_id sub_id = 0;
    };

    using engine_event = scheduled_event<tick_payload>;
    using engine_t     = scheduler<tick_payload>;

    // ── Engine event handler ───────────────────────────────────────────────────
    // Called on the engine's scheduler thread when an event fires.
    // Intentionally lightweight — the callback already ran by the time we
    // get here.  Reserved for future metrics / logging.
    void on_engine_event_(const engine_event& /*ev*/) {}

    // ── Schedule the next tick for a subscription ──────────────────────────────
    void schedule_next_(subscription_id id, time_point_t baseline) {
        double dt_ms = 0.0;
        {
            std::lock_guard<std::mutex> lock(subs_mutex_);
            auto it = subscriptions_.find(id);
            if (it == subscriptions_.end() || !it->second.active_) return;
            dt_ms = it->second.dt_ms_;
        }

        auto dt_dur  = std::chrono::duration_cast<duration_t>(
                           std::chrono::duration<double, std::milli>(dt_ms));

        time_point_t fire_at = baseline + dt_dur;

        engine_event ev;
        ev.member_time    = fire_at;
        ev.member_payload = tick_payload{id};

        // Self-rescheduling callback: fires the user tick, then returns
        // the next drift-free deadline (previous deadline + current dt).
        // dt is re-read from the subscription record each tick so that
        // update_dt() takes effect on the very next reschedule.
        ev.member_callback = [this, dt_ms, fire_at](tick_payload& payload) mutable
            -> std::optional<time_point_t>
        {
            tick_fn cb;
            double current_dt_ms = dt_ms;
            {
                std::lock_guard<std::mutex> lock(subs_mutex_);
                auto it = subscriptions_.find(payload.sub_id);
                if (it == subscriptions_.end() || !it->second.active_)
                    return std::nullopt;
                cb = it->second.callback_;
                current_dt_ms = it->second.dt_ms_;  // read live value
            }

            double t = now_ms();
            bool keep = cb(t, current_dt_ms);

            if (!keep) {
                std::lock_guard<std::mutex> lock(subs_mutex_);
                subscriptions_.erase(payload.sub_id);
                return std::nullopt;
            }

            // Drift-free: next deadline = previous deadline + current dt
            auto current_dt_dur = std::chrono::duration_cast<duration_t>(
                std::chrono::duration<double, std::milli>(current_dt_ms));
            fire_at += current_dt_dur;
            return fire_at;
        };

        engine_.add_event(std::move(ev));
    }

    // ── Members ────────────────────────────────────────────────────────────────

    engine_t       engine_;
    time_point_t   origin_;

    mutable std::mutex                                 subs_mutex_;
    std::unordered_map<subscription_id, subscription>  subscriptions_;
    std::atomic<subscription_id>                       next_id_{1};
};

} // namespace idyl::time