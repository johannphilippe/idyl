#pragma once

// =============================================================================
// audio_clock_scheduler.hpp — audio-callback-driven idyl scheduler (RtAudio)
//
// Uses a real audio device (via RtAudio) as a high-accuracy master clock.
// The audio driver calls the data callback at buffer-rate intervals:
//   128 frames / 48 kHz  →  ~2.67 ms per callback
//    32 frames / 48 kHz  →  ~0.67 ms per callback   (sub-millisecond)
//
// Architecture
// ────────────
//   audio callback  (RT thread) → increments atomic tick counter
//                               → releases a counting semaphore
//   worker thread               → acquires semaphore → fires due subscriptions
//
// The audio callback is minimal and real-time safe: no allocation, no locks,
// no heavy computation.  All idyl logic runs on the worker thread that wakes
// at buffer rate.
//
// Drift-free scheduling (same model as sys_clock_scheduler):
//   next_fire_ms = prev_fire_ms + dt_ms
// so timing stays accurate across many buffer callbacks.
// =============================================================================

#ifdef IDYL_AUDIO_CLOCK

#include "time/scheduler.hpp"

#include <RtAudio.h>

#include <atomic>
#include <cstring>
#include <iostream>
#include <mutex>
#include <semaphore>
#include <thread>
#include <unordered_map>

namespace idyl::time {

// ════════════════════════════════════════════════════════════════════════════════
// audio_clock_scheduler
// ════════════════════════════════════════════════════════════════════════════════

struct audio_clock_scheduler : idyl_scheduler {

    // ── Configuration ──────────────────────────────────────────────────────────
    unsigned int sample_rate_ = 48000;
    unsigned int buffer_size_ = 128;

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

        buffer_ms_ = static_cast<double>(buffer_size_) * 1000.0
                     / static_cast<double>(sample_rate_);

        unsigned int device_id = audio_.getDefaultOutputDevice();
        if (device_id == 0) {
            std::cerr << "[audio_clock] no output device available\n";
            return;
        }

        RtAudio::StreamParameters out_params;
        out_params.deviceId     = device_id;
        out_params.nChannels    = 1;
        out_params.firstChannel = 0;

        unsigned int buf = buffer_size_;

        RtAudioErrorType err = audio_.openStream(
            &out_params,
            nullptr,
            RTAUDIO_FLOAT32,
            sample_rate_,
            &buf,
            &audio_clock_scheduler::rt_callback_,
            this);

        if (err != RTAUDIO_NO_ERROR) {
            std::cerr << "[audio_clock] openStream failed (error " << err << ")\n";
            return;
        }

        // RtAudio may adjust the buffer size; re-derive buffer_ms_
        if (buf != buffer_size_) {
            buffer_size_ = buf;
            buffer_ms_   = static_cast<double>(buf) * 1000.0
                           / static_cast<double>(sample_rate_);
        }

        running_.store(true);
        tick_count_.store(0);
        worker_ = std::thread([this]{ worker_loop_(); });

        err = audio_.startStream();
        if (err != RTAUDIO_NO_ERROR) {
            std::cerr << "[audio_clock] startStream failed (error " << err << ")\n";
            running_.store(false);
            worker_sem_.release();
            if (worker_.joinable()) worker_.join();
            audio_.closeStream();
            return;
        }

        std::cerr << "[audio_clock] running at "
                  << sample_rate_ << " Hz, "
                  << buffer_size_ << " frames/buffer ("
                  << buffer_ms_ << " ms/tick)\n";
    }

    void stop() override {
        if (!running_.exchange(false)) return;

        if (audio_.isStreamRunning()) audio_.stopStream();
        if (audio_.isStreamOpen())    audio_.closeStream();

        worker_sem_.release();
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
        return static_cast<double>(tick_count_.load(std::memory_order_relaxed))
               * buffer_ms_;
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
    RtAudio audio_;
    double  buffer_ms_ = 0.0;

    std::atomic<bool>     running_{false};
    std::atomic<uint64_t> tick_count_{0};

    // Counting semaphore: allows a backlog of up to 64 unreleased audio ticks
    // if the worker is briefly slow — audio callback never blocks.
    std::counting_semaphore<64> worker_sem_{0};
    std::thread                 worker_;

    mutable std::mutex                               subs_mutex_;
    std::unordered_map<subscription_id, subs_entry>  subscriptions_;
    std::atomic<subscription_id>                     next_id_{1};

    // ── RtAudio callback ───────────────────────────────────────────────────────
    // Runs on the audio driver's real-time thread.
    // Kept minimal: output silence, advance tick counter, wake worker.
    static int rt_callback_(void*                output_buffer,
                             void*                /*input_buffer*/,
                             unsigned int         n_frames,
                             double               /*stream_time*/,
                             RtAudioStreamStatus  /*status*/,
                             void*                user_data)
    {
        auto* self = static_cast<audio_clock_scheduler*>(user_data);
        std::memset(output_buffer, 0, n_frames * sizeof(float));
        self->tick_count_.fetch_add(1, std::memory_order_release);
        self->worker_sem_.release();
        return 0;   // 0 = keep stream running
    }

    // ── Worker thread ──────────────────────────────────────────────────────────
    // Wakes at buffer rate; fires all subscriptions whose next_fire_ms_ has passed.
    void worker_loop_() {
        while (true) {
            worker_sem_.acquire();
            if (!running_.load(std::memory_order_relaxed)) break;

            const double now = now_ms();

            std::vector<subscription_id> due;
            {
                std::lock_guard<std::mutex> lk(subs_mutex_);
                for (const auto& [id, e] : subscriptions_)
                    if (e.active_ && now >= e.next_fire_ms_)
                        due.push_back(id);
            }

            for (subscription_id id : due) {
                tick_fn  cb;
                double   dt_ms        = 0.0;
                double   prev_fire_ms = 0.0;

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
    }
};

} // namespace idyl::time

#endif // IDYL_AUDIO_CLOCK
