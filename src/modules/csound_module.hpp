#pragma once

// =============================================================================
// csound_module.hpp — Csound integration for idyl
//
// Bridges idyl's temporal system with Csound instrument control.
// Each cs_open() call creates an independent Csound instance running its
// own performance thread, allowing multiple instances simultaneously.
//
// Functions:
//   handle = cs_open(path)                  compile + start a .csd or .orc file
//   cs_note(handle, instr, dur, p4, ...)    schedule an i-event (fires immediately)
//   cs_chnset(handle, channel, value)       write a named control channel
//   val = cs_chnget(handle, channel, dt=X)  temporal: poll a control channel
//   cs_close(handle)                        stop and destroy the instance
//
// cs_note:
//   instr  — instrument number (number) or instrument name (string)
//   dur    — duration in milliseconds (number or time value)
//   p4+    — additional p-fields passed verbatim
//   Returns trigger(true) on success.
//
// cs_chnget (native temporal):
//   Polls a Csound named control channel every dt milliseconds.
//   Output: current channel value as a number.
//   Emits:  "changed" trigger whenever the value changes between ticks.
//
// Thread safety:
//   csoundScoreEvent(), csoundSetControlChannel(), and
//   csoundGetControlChannel() are thread-safe in Csound 6+.
//   The performance thread holds no module-level locks.
//   A per-instance mutex guards only the CSOUND* pointer itself.
// =============================================================================

#include <string>
#include <unordered_map>
#include <memory>
#include <vector>
#include <mutex>
#include <thread>
#include <atomic>
#include <iostream>

#ifdef IDYL_MODULE_CSOUND
#include <csound/csound.h>
#endif

#include "include/module.hpp"

namespace idyl::module {

struct csound_module : base_module {

    // ── Per-instance state ──────────────────────────────────────────────────
    struct cs_instance {
        // The raw CSOUND* pointer.  Nulled under ptr_mutex before destruction.
#ifdef IDYL_MODULE_CSOUND
        CSOUND* cs = nullptr;
#endif
        std::atomic<bool> running{false};
        std::thread perf_thread;
        // Guards the cs pointer only — not the Csound API calls themselves,
        // which are thread-safe in Csound 6+.
        std::mutex ptr_mutex;

        ~cs_instance() { stop(); }

        void stop() {
#ifdef IDYL_MODULE_CSOUND
            running = false;
            if (perf_thread.joinable()) perf_thread.join();
            std::lock_guard<std::mutex> lock(ptr_mutex);
            if (cs) {
                csoundStop(cs);
                csoundDestroy(cs);
                cs = nullptr;
            }
#endif
        }

        // Thread-safe read of the cs pointer.
        // Caller must not store the returned pointer across a potential stop().
#ifdef IDYL_MODULE_CSOUND
        CSOUND* get() {
            std::lock_guard<std::mutex> lock(ptr_mutex);
            return cs;
        }
#endif
    };

    // ── Module state ────────────────────────────────────────────────────────
    std::unordered_map<intptr_t, std::shared_ptr<cs_instance>> instances_;
    intptr_t next_handle_ = 1;
    mutable std::mutex map_mutex_;  // guards instances_ map only

    // ── base_module interface ───────────────────────────────────────────────
    std::string name() const override { return "csound"; }

    bool available() const override {
#ifdef IDYL_MODULE_CSOUND
        return true;
#else
        return false;
#endif
    }

    std::string unavailable_reason() const override {
        return "Csound headers not available at build time "
               "(rebuild with -DIDYL_MODULE_CSOUND=ON)";
    }

    std::vector<function_entry> functions() override {

        // ── cs_open(path) → handle ─────────────────────────────────────────
        // Compile and start a Csound instance from a .csd or .orc file.
        // Returns a handle, or nil on failure.
        function_entry cs_open_e;
        cs_open_e.name_      = "cs_open";
        cs_open_e.fn_        = [this](span<const core::value> a) { return do_open(a); };
        cs_open_e.min_arity_ = 1;
        cs_open_e.max_arity_ = 1;

        // ── cs_note(handle, instr, dur_ms, p4, ...) → trigger ─────────────
        // Schedule a Csound i-event starting at the current moment (p2 = 0).
        //   instr:  instrument number or name string
        //   dur_ms: duration — number or time value (interpreted as ms)
        //   p4+:    additional Csound p-fields
        function_entry cs_note_e;
        cs_note_e.name_      = "cs_note";
        cs_note_e.fn_        = [this](span<const core::value> a) { return do_note(a); };
        cs_note_e.min_arity_ = 3;
        cs_note_e.max_arity_ = -1;

        // ── cs_chnset(handle, channel, value) → trigger ───────────────────
        // Write a value to a Csound named software-bus control channel.
        //   channel: channel name string (must be declared with chn_k in Csound)
        //   value:   number to write
        function_entry cs_chnset_e;
        cs_chnset_e.name_      = "cs_chnset";
        cs_chnset_e.fn_        = [this](span<const core::value> a) { return do_chnset(a); };
        cs_chnset_e.min_arity_ = 3;
        cs_chnset_e.max_arity_ = 3;

        // ── cs_chnget(handle, channel, dt=50ms) → number [temporal] ───────
        // Temporal: polls a named control channel on every tick.
        // Output: current value of the channel (number).
        // Emits:  "changed" trigger when the value differs from the previous tick.
        function_entry cs_chnget_e;
        cs_chnget_e.name_               = "cs_chnget";
        cs_chnget_e.is_native_temporal_ = true;
        cs_chnget_e.params_ = {
            required("handle"),
            required("channel"),
            with_default_ms("dt", 50.0),
        };

        cs_chnget_e.native_init_ = [](
            const core::native_state_t& /*params*/,
            core::native_state_t& state)
        {
            state["last_value"] = core::value::number(0.0);
        };

        cs_chnget_e.native_update_ = [this](
            const core::native_state_t& params,
            core::native_state_t& state,
            core::native_state_t& emitted,
            core::value& output)
        {
            auto h_it  = params.find("handle");
            auto ch_it = params.find("channel");
            if (h_it == params.end() || ch_it == params.end()) {
                output = core::value::number(0.0);
                return;
            }

            intptr_t h     = h_it->second.as_handle();
            std::string ch = ch_it->second.as_string();

            std::shared_ptr<cs_instance> inst = find_instance(h);
            if (!inst) { output = core::value::number(0.0); return; }

#ifdef IDYL_MODULE_CSOUND
            double val = 0.0;
            CSOUND* cs = inst->get();
            if (cs) {
                int err = 0;
                MYFLT v = csoundGetControlChannel(cs, ch.c_str(), &err);
                if (err == 0) val = static_cast<double>(v);
            }

            double prev = state["last_value"].as_number();
            core::value new_val = core::value::number(val);
            if (val != prev) {
                state["last_value"] = new_val;
                emitted["changed"]  = core::value::trigger(true);
            }
            output = new_val;
#else
            output = core::value::number(0.0);
#endif
        };

        // ── cs_close(handle) → trigger ────────────────────────────────────
        // Stop performance and destroy the Csound instance.
        function_entry cs_close_e;
        cs_close_e.name_      = "cs_close";
        cs_close_e.fn_        = [this](span<const core::value> a) { return do_close(a); };
        cs_close_e.min_arity_ = 1;
        cs_close_e.max_arity_ = 1;

        return {
            std::move(cs_open_e),
            std::move(cs_note_e),
            std::move(cs_chnset_e),
            std::move(cs_chnget_e),
            std::move(cs_close_e),
        };
    }

private:

    // ── Helpers ─────────────────────────────────────────────────────────────

    std::shared_ptr<cs_instance> find_instance(intptr_t h) const {
        std::lock_guard<std::mutex> lock(map_mutex_);
        auto it = instances_.find(h);
        return it != instances_.end() ? it->second : nullptr;
    }

    // ── cs_open ─────────────────────────────────────────────────────────────
    core::value do_open(span<const core::value> args) {
        if (args.size_ < 1) return core::value::nil();
        std::string path = args[0].as_string();
        if (path.empty()) return core::value::nil();

#ifdef IDYL_MODULE_CSOUND
        CSOUND* cs = csoundCreate(nullptr);
        if (!cs) {
            std::cerr << "[csound] csoundCreate failed\n";
            return core::value::nil();
        }

        csoundSetOption(cs, "-d");   // suppress console output; remove for debug

        const char* argv[] = { "csound", path.c_str() };
        int ret = csoundCompile(cs, 2, argv);
        if (ret != 0) {
            std::cerr << "[csound] compile failed: " << path << "\n";
            csoundDestroy(cs);
            return core::value::nil();
        }

        auto inst    = std::make_shared<cs_instance>();
        inst->cs     = cs;
        inst->running = true;

        // Performance loop — runs csoundPerformKsmps() as fast as Csound needs.
        // No module locks held: csoundPerformKsmps is not re-entrant but it runs
        // alone in this thread; all external calls (chnset/chnget/scoreEvent)
        // are thread-safe in Csound 6+.
        inst->perf_thread = std::thread([raw = inst.get()]() {
            while (raw->running.load(std::memory_order_relaxed)) {
                CSOUND* cs;
                {
                    std::lock_guard<std::mutex> lock(raw->ptr_mutex);
                    cs = raw->cs;
                }
                if (!cs) break;
                int rc = csoundPerformKsmps(cs);
                if (rc != 0) {
                    raw->running = false;
                    break;
                }
            }
        });

        intptr_t h = next_handle_++;
        {
            std::lock_guard<std::mutex> lock(map_mutex_);
            instances_.emplace(h, std::move(inst));
        }
        return core::value::handle(h);
#else
        std::cerr << "[csound] module not compiled with Csound support\n";
        return core::value::nil();
#endif
    }

    // ── cs_note ─────────────────────────────────────────────────────────────
    // cs_note(handle, instr, dur_ms, p4, p5, ...)
    core::value do_note(span<const core::value> args) {
        if (args.size_ < 3) return core::value::trigger(false);

        intptr_t h = args[0].as_handle();
        // dur: both number_ and time value store ms in number_
        double dur_s = args[2].number_ / 1000.0;

        std::shared_ptr<cs_instance> inst = find_instance(h);
        if (!inst) return core::value::trigger(false);

#ifdef IDYL_MODULE_CSOUND
        CSOUND* cs = inst->get();
        if (!cs) return core::value::trigger(false);

        if (args[1].type_ == core::value_t::string) {
            // Named instrument: build a score string and feed it via csoundReadScore.
            // Format: i "name" 0 dur_s [p4 p5 ...]
            std::string score = "i \"" + args[1].as_string() + "\" 0 "
                                + std::to_string(dur_s);
            for (size_t i = 3; i < args.size_; ++i)
                score += " " + std::to_string(args[i].as_number());
            score += "\n";
            csoundReadScoreAsync(cs, score.c_str());

        } else {
            // Numeric instrument: use csoundScoreEvent for efficiency.
            // p-field layout: p1=instr, p2=0 (now), p3=dur_s, p4+...
            std::vector<MYFLT> pf;
            pf.reserve(3 + args.size_ - 3);
            pf.push_back(static_cast<MYFLT>(args[1].as_number())); // p1
            pf.push_back(0.0);                                       // p2 = now
            pf.push_back(static_cast<MYFLT>(dur_s));                 // p3
            for (size_t i = 3; i < args.size_; ++i)
                pf.push_back(static_cast<MYFLT>(args[i].as_number()));

            csoundScoreEventAsync(cs, 'i', pf.data(), static_cast<long>(pf.size()));
        }
        return core::value::trigger(true);
#else
        return core::value::trigger(false);
#endif
    }

    // ── cs_chnset ───────────────────────────────────────────────────────────
    // cs_chnset(handle, channel, value)
    core::value do_chnset(span<const core::value> args) {
        if (args.size_ < 3) return core::value::trigger(false);

        intptr_t    h   = args[0].as_handle();
        std::string ch  = args[1].as_string();
        double      val = args[2].as_number();

        std::shared_ptr<cs_instance> inst = find_instance(h);
        if (!inst) return core::value::trigger(false);

#ifdef IDYL_MODULE_CSOUND
        CSOUND* cs = inst->get();
        if (!cs) return core::value::trigger(false);
        csoundSetControlChannel(cs, ch.c_str(), static_cast<MYFLT>(val));
        return core::value::trigger(true);
#else
        return core::value::trigger(false);
#endif
    }

    // ── cs_close ────────────────────────────────────────────────────────────
    core::value do_close(span<const core::value> args) {
        if (args.size_ < 1) return core::value::trigger(false);
        intptr_t h = args[0].as_handle();

        std::shared_ptr<cs_instance> inst;
        {
            std::lock_guard<std::mutex> lock(map_mutex_);
            auto it = instances_.find(h);
            if (it == instances_.end()) return core::value::trigger(false);
            inst = std::move(it->second);
            instances_.erase(it);
        }
        // stop() joins the performance thread outside the map lock.
        inst->stop();
        return core::value::trigger(true);
    }
};

} // namespace idyl::module
