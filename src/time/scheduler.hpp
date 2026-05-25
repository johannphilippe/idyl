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
#include <optional>
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
    subscription_id  id_             = 0;
    double           dt_ms_          = 0.0;
    tick_fn          callback_;
    bool             active_         = true;
    std::string      process_tag_;           // owning process name (empty = unowned)
    bool             paused_         = false;
    time_point_t     last_fire_at_   = {};  // deadline of the last successful tick
    uint64_t         armed_generation_ = 0; // incremented on each arm; stale lambdas exit
};

// ════════════════════════════════════════════════════════════════════════════════
// idyl_scheduler — abstract interface shared by all scheduler implementations
//
// Concrete types: sys_clock_scheduler (default), audio_clock_scheduler
// (enabled with --audio-clock CLI flag).
// ════════════════════════════════════════════════════════════════════════════════
struct idyl_scheduler {
    virtual ~idyl_scheduler() = default;

    virtual void            start()                                              = 0;
    virtual void            stop()                                               = 0;
    virtual bool            is_running()   const noexcept                        = 0;

    virtual subscription_id subscribe(double dt_ms, tick_fn callback,
                                      std::string process_tag = "")              = 0;
    virtual void            unsubscribe(subscription_id id)                      = 0;
    virtual void            update_dt(subscription_id id, double new_dt_ms)      = 0;

    virtual void            pause_process(const std::string& name)               = 0;
    virtual void            resume_process(const std::string& name)              = 0;

    virtual double          now_ms()       const noexcept                        = 0;
    virtual std::size_t     active_count() const noexcept                        = 0;
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

struct sys_clock_scheduler : idyl_scheduler {

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

    void start() override {
        origin_ = steady_clock_t::now();
        engine_.start();
    }

    void stop() override {
        engine_.stop();

        std::lock_guard<std::mutex> lock(subs_mutex_);
        subscriptions_.clear();
    }

    bool is_running() const noexcept override { return engine_.is_running(); }

    // ── Subscribe / unsubscribe ────────────────────────────────────────────────

    // Register a periodic callback with the given tick interval.
    // process_tag, if non-empty, associates this subscription with a named
    // process so that pause_process() / resume_process() can target it.
    subscription_id subscribe(double dt_ms, tick_fn callback,
                              std::string process_tag = "") override {
        subscription_id id = next_id_++;

        subscription sub;
        sub.id_          = id;
        sub.dt_ms_       = dt_ms;
        sub.callback_    = std::move(callback);
        sub.active_      = true;
        sub.process_tag_ = process_tag;

        std::cerr << "[sched:subscribe] id=" << id
                  << " dt=" << dt_ms << "ms tag='" << process_tag << "'\n";

        {
            std::lock_guard<std::mutex> lock(subs_mutex_);
            subscriptions_[id] = std::move(sub);
        }

        arm_subscription_(id, steady_clock_t::now());
        return id;
    }

    // Remove a subscription. Safe to call from any thread, including
    // from inside a tick callback (deferred removal).
    void unsubscribe(subscription_id id) override {
        std::lock_guard<std::mutex> lock(subs_mutex_);
        auto it = subscriptions_.find(id);
        if (it != subscriptions_.end()) {
            it->second.active_ = false;   // engine will skip & drop
        }
    }

    // Update the tick interval of an existing subscription.
    // Takes effect on the next reschedule (after the current tick completes).
    void update_dt(subscription_id id, double new_dt_ms) override {
        std::lock_guard<std::mutex> lock(subs_mutex_);
        auto it = subscriptions_.find(id);
        if (it != subscriptions_.end())
            it->second.dt_ms_ = new_dt_ms;
    }

    // Pause all subscriptions tagged with name.  Their lambdas will detect the
    // paused_ flag on the next fire, save their current fire_at, and return
    // nullopt — removing themselves from the engine queue while staying in the
    // subscriptions_ map.
    void pause_process(const std::string& name) override {
        std::lock_guard<std::mutex> lock(subs_mutex_);
        for (auto& [id, sub] : subscriptions_) {
            if (sub.process_tag_ == name && sub.active_ && !sub.paused_) {
                sub.paused_ = true;
                std::cerr << "[sched:pause] id=" << id
                          << " dt=" << sub.dt_ms_ << "ms last_fire_at="
                          << (sub.last_fire_at_ == time_point_t{} ? "never"
                              : std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(
                                    sub.last_fire_at_ - origin_).count()) + "ms")
                          << "\n";
            }
        }
        paused_at_[name] = steady_clock_t::now();
    }

    // Resume all subscriptions tagged with name.
    // Uses phase-preserving timing: the first tick fires at expected_next + pause_dur,
    // where expected_next = last_fire_at_ + dt.  This ensures an each-loop subscription
    // always fires before the outer metro subscription (same ordering as before the
    // pause), so the each-loop plays its remaining iterations without overlap.
    void resume_process(const std::string& name) override {
        time_point_t resume_at = steady_clock_t::now();

        // Collect baselines under the lock, then arm outside it
        // (arm_subscription_ acquires subs_mutex_ internally).
        std::vector<std::pair<subscription_id, time_point_t>> to_arm;
        {
            std::lock_guard<std::mutex> lock(subs_mutex_);
            auto pause_it = paused_at_.find(name);
            if (pause_it == paused_at_.end()) return;
            auto pause_dur = resume_at - pause_it->second;
            paused_at_.erase(pause_it);

            for (auto& [id, sub] : subscriptions_) {
                if (sub.process_tag_ == name && sub.active_ && sub.paused_) {
                    sub.paused_ = false;
                    // Pre-invalidate the old lambda NOW, while we hold the lock.
                    sub.armed_generation_++;

                    auto dt_dur = std::chrono::duration_cast<duration_t>(
                        std::chrono::duration<double, std::milli>(sub.dt_ms_));

                    time_point_t baseline;
                    if (sub.last_fire_at_ != time_point_t{}) {
                        baseline = sub.last_fire_at_ + pause_dur;
                    } else {
                        baseline = resume_at - dt_dur;
                    }

                    auto fire_at_ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                        (baseline + dt_dur) - origin_).count();
                    auto now_ms_val = std::chrono::duration_cast<std::chrono::milliseconds>(
                        resume_at - origin_).count();
                    std::cerr << "[sched:resume] id=" << id
                              << " dt=" << sub.dt_ms_ << "ms"
                              << " fire_at=" << fire_at_ms << "ms"
                              << " now=" << now_ms_val << "ms"
                              << " gen=" << sub.armed_generation_ << "\n";

                    to_arm.emplace_back(id, baseline);
                }
            }
        }

        for (auto& [id, baseline] : to_arm)
            arm_subscription_(id, baseline);
    }

    // ── Queries ────────────────────────────────────────────────────────────────

    // Elapsed time since start() in milliseconds.
    double now_ms() const noexcept override {
        auto elapsed = steady_clock_t::now() - origin_;
        return std::chrono::duration<double, std::milli>(elapsed).count();
    }

    std::size_t active_count() const noexcept override {
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

    // ── Arm a subscription: schedule its next tick at fire_at + dt ───────────────
    // Used both by subscribe() (initial arm) and resume_process() (re-arm).
    // Caller must NOT hold subs_mutex_.
    void arm_subscription_(subscription_id id, time_point_t baseline) {
        double   dt_ms = 0.0;
        uint64_t gen   = 0;
        {
            std::lock_guard<std::mutex> lock(subs_mutex_);
            auto it = subscriptions_.find(id);
            if (it == subscriptions_.end() || !it->second.active_) return;
            dt_ms = it->second.dt_ms_;
            // Increment generation so any older lambda in the engine queue will
            // detect the mismatch and exit without firing.
            gen = ++it->second.armed_generation_;
        }

        auto dt_dur  = std::chrono::duration_cast<duration_t>(
                           std::chrono::duration<double, std::milli>(dt_ms));

        time_point_t fire_at = baseline + dt_dur;

        engine_event ev;
        ev.member_time    = fire_at;
        ev.member_payload = tick_payload{id};

        // Self-rescheduling callback.
        // On each fire:
        //   1. If inactive → erase and return nullopt.
        //   2. If generation mismatch → stale lambda (superseded by resume re-arm) → exit.
        //   3. If paused  → return nullopt (resume re-arms with immediate baseline)
        //                    (subscription stays in map; resume re-arms it).
        //   4. Normal tick → invoke callback, reschedule at next deadline.
        std::cerr << "[sched:arm] id=" << id << " gen=" << gen
                  << " fire_at=" << std::chrono::duration_cast<std::chrono::milliseconds>(
                         fire_at - origin_).count() << "ms\n";

        ev.member_callback = [this, dt_ms, fire_at, gen](tick_payload& payload) mutable
            -> std::optional<time_point_t>
        {
            tick_fn cb;
            double current_dt_ms = dt_ms;
            {
                std::lock_guard<std::mutex> lock(subs_mutex_);
                auto it = subscriptions_.find(payload.sub_id);
                if (it == subscriptions_.end() || !it->second.active_) {
                    std::cerr << "[sched:lambda] id=" << payload.sub_id << " gen=" << gen << " → inactive\n";
                    return std::nullopt;
                }

                // Stale lambda: a newer arm superseded us (e.g. resume re-arm).
                if (it->second.armed_generation_ != gen) {
                    std::cerr << "[sched:lambda] id=" << payload.sub_id << " gen=" << gen
                              << " → stale (armed_gen=" << it->second.armed_generation_ << ")\n";
                    return std::nullopt;
                }

                // Paused: save the deadline we would have fired at and leave
                // the engine queue (subscription stays alive in the map).
                if (it->second.paused_) {
                    std::cerr << "[sched:lambda] id=" << payload.sub_id << " gen=" << gen << " → paused\n";
                    return std::nullopt;
                }

                std::cerr << "[sched:lambda] id=" << payload.sub_id << " gen=" << gen << " → FIRE\n";

                // Record the deadline of this successful tick so resume_process
                // can compute expected_next = last_fire_at_ + dt even when the
                // lambda hasn't had a chance to detect the pause yet (quick pause).
                it->second.last_fire_at_ = fire_at;
                cb = it->second.callback_;
                current_dt_ms = it->second.dt_ms_;
            }

            double t = now_ms();
            bool keep = cb(t, current_dt_ms);

            if (!keep) {
                std::lock_guard<std::mutex> lock(subs_mutex_);
                subscriptions_.erase(payload.sub_id);
                return std::nullopt;
            }

            // Re-read dt after the callback so update_dt() takes effect.
            {
                std::lock_guard<std::mutex> lock(subs_mutex_);
                auto it2 = subscriptions_.find(payload.sub_id);
                if (it2 != subscriptions_.end())
                    current_dt_ms = it2->second.dt_ms_;
            }

            // Drift-free: next deadline = previous deadline + current dt.
            // Skip ahead by whole periods if we fell behind (avoids burst
            // catch-up firing that causes audible duplicate notes).
            auto current_dt_dur = std::chrono::duration_cast<duration_t>(
                std::chrono::duration<double, std::milli>(current_dt_ms));
            fire_at += current_dt_dur;
            auto now_tp = steady_clock_t::now();
            while (fire_at <= now_tp)
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
    std::unordered_map<std::string, time_point_t>      paused_at_;
};

} // namespace idyl::time