#pragma once

// =============================================================================
// osc_module.hpp — OSC (Open Sound Control) module for idyl
//
// Exposes OSC sender/receiver functionality to idyl programs.
//
// Functions:
//   osc_out(host, port)          → configure the output endpoint
//   osc_send(address, args...)   → send an OSC message (see below)
//   osc_in(port)                 → configure the input endpoint (listener)
//   osc_stop()                   → stop all scheduled (dt-driven) sends
//   osc_close()                  → close all connections + stop scheduled sends
//
// osc_send modes (user's choice):
//   osc_send("/addr", 440)                 → immediate one-shot send
//   osc_send("/addr", 440, 100ms)          → dt-driven: repeats every 100ms
//   osc_send("/addr", 440, !)              → trigger-gated: sends only when gate is true
//   osc_send("/addr", 440, 100ms, !)       → both: repeats at dt, gated by trigger
//
// In a reactive process block, osc_send (without dt) re-fires on every tick
// of the enclosing temporal source — so you get dt-driven sends for free:
//
//   process: {
//       c = counter(100ms)
//       osc_send("/count", c)     // fires every 100ms with latest c
//   }
//
// =============================================================================

#include "include/module.hpp"
#include "utilities/osc.hpp"
#include "utilities/udp.hpp"
#include "time/scheduler.hpp"

#include <memory>
#include <mutex>
#include <iostream>
#include <vector>

namespace idyl::modules {

    struct osc_module : module::base_module {

        // ── State ──────────────────────────────────────────────────────────
        std::unique_ptr<utilities::udp_sender>   sender_;
        std::unique_ptr<utilities::udp_receiver> receiver_;
        mutable std::mutex mutex_;    // protects sender_/receiver_ access

        // Scheduler (provided via set_scheduler)
        time::sys_clock_scheduler* scheduler_ = nullptr;

        // Active dt-driven subscriptions (for cleanup)
        std::vector<time::subscription_id> active_subs_;

        // ── base_module interface ──────────────────────────────────────────
        std::string name() const override { return "osc"; }

        bool available() const override {
            #ifdef IDYL_MODULE_OSC
            return true;
            #else
            return false;
            #endif
        }

        std::string unavailable_reason() const override {
            return "OSC module was disabled at build time (cmake -DIDYL_MODULE_OSC=OFF)";
        }

        void set_scheduler(time::sys_clock_scheduler* sched) override {
            scheduler_ = sched;
        }

        std::vector<module::function_entry> functions() override {
            return {
                {
                    "osc_out",
                    [this](span<const core::value> args) -> core::value {
                        return do_osc_out(args);
                    },
                    2, 2   // osc_out(host, port)
                },
                {
                    "osc_send",
                    [this](span<const core::value> args) -> core::value {
                        return do_osc_send(args);
                    },
                    1, -1  // osc_send(address, ...args) — variadic
                },
                {
                    "osc_in",
                    [this](span<const core::value> args) -> core::value {
                        return do_osc_in(args);
                    },
                    1, 1   // osc_in(port)
                },
                {
                    "osc_stop",
                    [this](span<const core::value> /*args*/) -> core::value {
                        return do_osc_stop();
                    },
                    0, 0   // osc_stop()
                },
                {
                    "osc_close",
                    [this](span<const core::value> /*args*/) -> core::value {
                        return do_osc_close();
                    },
                    0, 0   // osc_close()
                },
            };
        }

    private:

        // ── osc_out(host: string, port: number) ───────────────────────────
        core::value do_osc_out(span<const core::value> args) {
            std::string host = args[0].as_string();
            int port = static_cast<int>(args[1].as_number());

            if (host.empty() || port <= 0 || port > 65535) {
                std::cerr << "[osc] osc_out: invalid host or port\n";
                return core::value::trigger(false);
            }

            try {
                std::lock_guard<std::mutex> lock(mutex_);
                sender_ = std::make_unique<utilities::udp_sender>(
                    utilities::endpoint{host, static_cast<uint16_t>(port)});
                return core::value::trigger(true);
            } catch (const std::exception& e) {
                std::cerr << "[osc] osc_out: " << e.what() << "\n";
                return core::value::trigger(false);
            }
        }

        // ── osc_send(address, ...args, [dt], [gate]) ─────────────────────
        //
        // Trailing argument inspection:
        //   - If the last arg is a trigger → it's a gate (send only if true)
        //   - If the arg before the gate (or last) is a time → it's a dt
        //   - Everything else is an OSC payload argument.
        //
        // This means:
        //   osc_send("/a", 440)                → immediate
        //   osc_send("/a", 440, 100ms)         → dt-driven (100ms repeat)
        //   osc_send("/a", 440, !)             → gated (send if trigger true)
        //   osc_send("/a", 440, 100ms, !)      → dt-driven + gated
        //
        core::value do_osc_send(span<const core::value> args) {
            if (args.size_ < 1) return core::value::trigger(false);

            std::lock_guard<std::mutex> lock(mutex_);
            if (!sender_) {
                std::cerr << "[osc] osc_send: no output configured (call osc_out first)\n";
                return core::value::trigger(false);
            }

            // ── Parse trailing control args ────────────────────────────────
            size_t n = args.size_;
            size_t osc_end = n;      // exclusive end of OSC payload args
            double dt_ms   = 0.0;
            bool   has_dt  = false;
            bool   has_gate = false;
            bool   gate    = true;

            // Check last arg for trigger (gate)
            if (osc_end > 1 && args[osc_end - 1].type_ == core::value_t::trigger) {
                has_gate = true;
                gate = args[osc_end - 1].trigger_;
                --osc_end;
            }

            // Check (now-)last arg for time (dt)
            if (osc_end > 1 && args[osc_end - 1].type_ == core::value_t::time) {
                has_dt = true;
                dt_ms = args[osc_end - 1].number_;
                --osc_end;
            }

            // If gated and gate is false → skip (both immediate and scheduled)
            if (has_gate && !gate) {
                return core::value::trigger(false);
            }

            // ── Build OSC message ──────────────────────────────────────────
            std::string address = args[0].as_string();
            if (address.empty() || address[0] != '/') {
                std::cerr << "[osc] osc_send: address must start with '/'\n";
                return core::value::trigger(false);
            }

            auto build_msg = [&]() -> osc_message {
                osc_message msg(address);
                for (size_t i = 1; i < osc_end; ++i) {
                    const auto& v = args[i];
                    switch (v.type_) {
                        case core::value_t::number:
                            msg << static_cast<float>(v.number_);
                            break;
                        case core::value_t::string:
                            msg << v.as_string();
                            break;
                        case core::value_t::time:
                            msg << static_cast<float>(v.number_);
                            break;
                        case core::value_t::trigger:
                            msg << v.trigger_;
                            break;
                        default:
                            msg << osc_message::nil_t{};
                            break;
                    }
                }
                return msg;
            };

            // ── dt-driven: schedule periodic sends ─────────────────────────
            if (has_dt && dt_ms > 0.0 && scheduler_) {
                // Serialize the message once — periodic sends repeat the same payload.
                // For dynamic values, use a reactive process block instead.
                auto bytes = build_msg().serialise();

                // Capture a raw pointer to sender — module lifetime > scheduler lifetime.
                auto* raw_sender = sender_.get();

                auto sub_id = scheduler_->subscribe(dt_ms,
                    [raw_sender, bytes, &mtx = mutex_]
                    (double /*t*/, double /*dt*/) -> bool {
                        try {
                            std::lock_guard<std::mutex> lock(mtx);
                            if (raw_sender) {
                                raw_sender->send(bytes);
                            }
                        } catch (...) {}
                        return true; // keep alive
                    });

                active_subs_.push_back(sub_id);
                return core::value::trigger(true);
            }

            // ── Immediate send ─────────────────────────────────────────────
            try {
                build_msg().send(*sender_);
                return core::value::trigger(true);
            } catch (const std::exception& e) {
                std::cerr << "[osc] osc_send: " << e.what() << "\n";
                return core::value::trigger(false);
            }
        }

        // ── osc_in(port: number) ──────────────────────────────────────────
        core::value do_osc_in(span<const core::value> args) {
            int port = static_cast<int>(args[0].as_number());
            if (port <= 0 || port > 65535) {
                std::cerr << "[osc] osc_in: invalid port\n";
                return core::value::trigger(false);
            }

            try {
                std::lock_guard<std::mutex> lock(mutex_);
                receiver_ = std::make_unique<utilities::udp_receiver>(
                    static_cast<uint16_t>(port));
                return core::value::trigger(true);
            } catch (const std::exception& e) {
                std::cerr << "[osc] osc_in: " << e.what() << "\n";
                return core::value::trigger(false);
            }
        }

        // ── osc_stop() ───────────────────────────────────────────────────
        // Stop all dt-driven scheduled sends.
        core::value do_osc_stop() {
            if (scheduler_) {
                for (auto id : active_subs_)
                    scheduler_->unsubscribe(id);
            }
            active_subs_.clear();
            return core::value::trigger(true);
        }

        // ── osc_close() ──────────────────────────────────────────────────
        // Stops scheduled sends + closes sender/receiver.
        core::value do_osc_close() {
            do_osc_stop();
            std::lock_guard<std::mutex> lock(mutex_);
            sender_.reset();
            receiver_.reset();
            return core::value::trigger(true);
        }
    };

} // namespace idyl::modules
