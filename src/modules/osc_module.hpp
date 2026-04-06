#pragma once

// =============================================================================
// osc_module.hpp — OSC (Open Sound Control) module for idyl
//
// Exposes OSC sender/receiver functionality to idyl programs.
// All connections are managed via opaque handles, allowing multiple
// simultaneous OSC endpoints.
//
// Functions:
//   handle = osc_out(host, port)       → open a UDP output endpoint, return handle
//   handle = osc_in(port)              → open a UDP input listener, return handle
//   osc_send(handle, address, args...) → send an OSC message on the given output
//   osc_close(handle)                  → close a specific connection
//   osc_stop()                         → stop all scheduled (dt-driven) sends
//
// osc_send modes (trailing argument inspection):
//   osc_send(h, "/addr", 440)                 → immediate one-shot send
//   osc_send(h, "/addr", 440, 100ms)          → dt-driven: repeats every 100ms
//   osc_send(h, "/addr", 440, !)              → trigger-gated: sends only on bang
//   osc_send(h, "/addr", 440, 100ms, !)       → both: repeats at dt, gated
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
#include <unordered_map>

namespace idyl::modules {

    struct osc_module : module::base_module {

        // ── Connection types ───────────────────────────────────────────────
        enum class conn_type { sender, receiver };

        struct connection {
            conn_type type_;
            std::unique_ptr<utilities::udp_sender>   sender_;
            std::unique_ptr<utilities::udp_receiver> receiver_;
        };

        // ── State ──────────────────────────────────────────────────────────
        std::unordered_map<intptr_t, std::unique_ptr<connection>> connections_;
        intptr_t next_handle_ = 1;  // monotonically increasing handle IDs
        mutable std::mutex mutex_;

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
                    2, 2   // osc_out(host, port) → handle
                },
                {
                    "osc_send",
                    [this](span<const core::value> args) -> core::value {
                        return do_osc_send(args);
                    },
                    2, -1  // osc_send(handle, address, ...args) — variadic
                },
                {
                    "osc_in",
                    [this](span<const core::value> args) -> core::value {
                        return do_osc_in(args);
                    },
                    1, 1   // osc_in(port) → handle
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
                    [this](span<const core::value> args) -> core::value {
                        return do_osc_close(args);
                    },
                    1, 1   // osc_close(handle)
                },
            };
        }

    private:

        // ── osc_out(host: string, port: number) → handle ─────────────────
        core::value do_osc_out(span<const core::value> args) {
            std::string host = args[0].as_string();
            int port = static_cast<int>(args[1].as_number());

            if (host.empty() || port <= 0 || port > 65535) {
                std::cerr << "[osc] osc_out: invalid host or port\n";
                return core::value::nil();
            }

            try {
                std::lock_guard<std::mutex> lock(mutex_);
                auto conn = std::make_unique<connection>();
                conn->type_ = conn_type::sender;
                conn->sender_ = std::make_unique<utilities::udp_sender>(
                    utilities::endpoint{host, static_cast<uint16_t>(port)});
                intptr_t h = next_handle_++;
                connections_.emplace(h, std::move(conn));
                return core::value::handle(h);
            } catch (const std::exception& e) {
                std::cerr << "[osc] osc_out: " << e.what() << "\n";
                return core::value::nil();
            }
        }

        // ── osc_send(handle, address, ...args, [dt], [gate]) ─────────────
        //
        // First arg must be a handle (from osc_out).
        // Second arg is the OSC address string.
        // Trailing argument inspection (same as before):
        //   - If the last arg is a trigger → it's a gate
        //   - If the arg before the gate (or last) is a time → it's a dt
        //   - Everything else is an OSC payload argument.
        //
        core::value do_osc_send(span<const core::value> args) {
            if (args.size_ < 2) return core::value::trigger(false);

            intptr_t h = args[0].as_handle();

            std::lock_guard<std::mutex> lock(mutex_);
            auto it = connections_.find(h);
            if (it == connections_.end() || !it->second ||
                it->second->type_ != conn_type::sender || !it->second->sender_) {
                std::cerr << "[osc] osc_send: invalid or closed handle\n";
                return core::value::trigger(false);
            }
            auto* sender = it->second->sender_.get();

            // ── Parse trailing control args ────────────────────────────────
            size_t n = args.size_;
            size_t osc_end = n;      // exclusive end of OSC payload args
            double dt_ms   = 0.0;
            bool   has_dt  = false;
            bool   has_gate = false;
            bool   gate    = true;

            // Check last arg for trigger (gate)
            if (osc_end > 2 && args[osc_end - 1].type_ == core::value_t::trigger) {
                has_gate = true;
                gate = args[osc_end - 1].trigger_;
                --osc_end;
            }

            // Check (now-)last arg for time (dt)
            if (osc_end > 2 && args[osc_end - 1].type_ == core::value_t::time) {
                has_dt = true;
                dt_ms = args[osc_end - 1].number_;
                --osc_end;
            }

            // If gated and gate is false → skip
            if (has_gate && !gate) {
                return core::value::trigger(false);
            }

            // ── Build OSC message ──────────────────────────────────────────
            std::string address = args[1].as_string();
            if (address.empty() || address[0] != '/') {
                std::cerr << "[osc] osc_send: address must start with '/'\n";
                return core::value::trigger(false);
            }

            auto build_msg = [&]() -> osc_message {
                osc_message msg(address);
                for (size_t i = 2; i < osc_end; ++i) {
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
                auto bytes = build_msg().serialise();
                auto* raw_sender = sender;

                auto sub_id = scheduler_->subscribe(dt_ms,
                    [raw_sender, bytes, &mtx = mutex_]
                    (double /*t*/, double /*dt*/) -> bool {
                        try {
                            std::lock_guard<std::mutex> lock(mtx);
                            if (raw_sender) {
                                raw_sender->send(bytes);
                            }
                        } catch (...) {}
                        return true;
                    });

                active_subs_.push_back(sub_id);
                return core::value::trigger(true);
            }

            // ── Immediate send ─────────────────────────────────────────────
            try {
                build_msg().send(*sender);
                return core::value::trigger(true);
            } catch (const std::exception& e) {
                std::cerr << "[osc] osc_send: " << e.what() << "\n";
                return core::value::trigger(false);
            }
        }

        // ── osc_in(port: number) → handle ─────────────────────────────────
        core::value do_osc_in(span<const core::value> args) {
            int port = static_cast<int>(args[0].as_number());
            if (port <= 0 || port > 65535) {
                std::cerr << "[osc] osc_in: invalid port\n";
                return core::value::nil();
            }

            try {
                std::lock_guard<std::mutex> lock(mutex_);
                auto conn = std::make_unique<connection>();
                conn->type_ = conn_type::receiver;
                conn->receiver_ = std::make_unique<utilities::udp_receiver>(
                    static_cast<uint16_t>(port));
                intptr_t h = next_handle_++;
                connections_.emplace(h, std::move(conn));
                return core::value::handle(h);
            } catch (const std::exception& e) {
                std::cerr << "[osc] osc_in: " << e.what() << "\n";
                return core::value::nil();
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

        // ── osc_close(handle) ────────────────────────────────────────────
        // Closes a specific connection by handle.
        core::value do_osc_close(span<const core::value> args) {
            intptr_t h = args[0].as_handle();
            std::lock_guard<std::mutex> lock(mutex_);
            auto it = connections_.find(h);
            if (it == connections_.end()) {
                std::cerr << "[osc] osc_close: unknown handle\n";
                return core::value::trigger(false);
            }
            connections_.erase(it);
            return core::value::trigger(true);
        }
    };

} // namespace idyl::modules
