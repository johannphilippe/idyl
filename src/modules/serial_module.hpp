#pragma once

// =============================================================================
// serial_module.hpp — Serial port module for idyl
//
// Exposes serial port I/O to idyl programs via opaque handles.
// Platform-portable: Linux/macOS (POSIX termios) and Windows (Win32 API).
//
// Functions:
//   handle = serial_open(port, baud)   → open a serial port, return handle
//   serial_close(handle)               → close the port
//   serial_write(handle, data)         → write string to port
//   data = serial_recv(handle, dt=10ms)→ native temporal: polls port each tick,
//                                        output is received string (or ""),
//                                        emits "received" trigger when data arrives
// =============================================================================

#include "include/module.hpp"
#include "utilities/serial.hpp"

#include <memory>
#include <mutex>
#include <iostream>
#include <unordered_map>

namespace idyl::modules {

    struct serial_module : module::base_module {

        // ── State ──────────────────────────────────────────────────────────
        std::unordered_map<intptr_t, std::unique_ptr<utilities::serial_port>> ports_;
        intptr_t next_handle_ = 1;
        mutable std::mutex mutex_;

        // ── base_module interface ──────────────────────────────────────────
        std::string name() const override { return "serial"; }

        bool available() const override {
            #ifdef IDYL_MODULE_SERIAL
            return true;
            #else
            return false;
            #endif
        }

        std::string unavailable_reason() const override {
            return "Serial module was disabled at build time (cmake -DIDYL_MODULE_SERIAL=OFF)";
        }

        void cleanup() noexcept override {
            std::lock_guard<std::mutex> lock(mutex_);
            ports_.clear();
        }

        std::vector<module::function_entry> functions() override {
            using namespace module;

            function_entry open_entry;
            open_entry.name_      = "serial_open";
            open_entry.fn_        = [this](span<const core::value> args) { return do_open(args); };
            open_entry.min_arity_ = 2;
            open_entry.max_arity_ = 2;

            function_entry close_entry;
            close_entry.name_      = "serial_close";
            close_entry.fn_        = [this](span<const core::value> args) { return do_close(args); };
            close_entry.min_arity_ = 1;
            close_entry.max_arity_ = 1;

            function_entry write_entry;
            write_entry.name_      = "serial_write";
            write_entry.fn_        = [this](span<const core::value> args) { return do_write(args); };
            write_entry.min_arity_ = 2;
            write_entry.max_arity_ = 2;

            // serial_recv: native temporal — polls the port each tick
            // Usage: data = serial_recv(handle, dt=10ms)
            // Output: string of received bytes, or "" if nothing arrived.
            // Emits: "received" trigger whenever bytes arrive.
            function_entry recv_entry;
            recv_entry.name_              = "serial_recv";
            recv_entry.is_native_temporal_ = true;
            recv_entry.params_ = {
                required("handle"),
                with_default_ms("dt", 10.0),
            };

            recv_entry.native_init_ = [](
                const core::native_state_t& /*params*/,
                core::native_state_t& state)
            {
                state["last_data"] = core::value::string("");
            };

            recv_entry.native_update_ = [this](
                const core::native_state_t& params,
                core::native_state_t& state,
                core::native_state_t& emitted,
                core::value& output)
            {
                intptr_t h = 0;
                auto hit = params.find("handle");
                if (hit != params.end()) h = hit->second.as_handle();

                if (h == 0) { output = core::value::string(""); return; }

                std::lock_guard<std::mutex> lock(mutex_);
                auto pit = ports_.find(h);
                if (pit == ports_.end() || !pit->second || !pit->second->is_open()) {
                    output = core::value::string("");
                    return;
                }

                std::string data = pit->second->read_available();
                if (data.empty()) {
                    output = state["last_data"];
                    return;
                }

                state["last_data"] = core::value::string(data);
                emitted["received"] = core::value::trigger(true);
                output = core::value::string(data);
            };

            return {
                std::move(open_entry),
                std::move(close_entry),
                std::move(write_entry),
                std::move(recv_entry),
            };
        }

    private:

        // ── serial_open(port: string, baud: number) → handle ──────────────
        core::value do_open(span<const core::value> args) {
            std::string port = args[0].as_string();
            int baud = static_cast<int>(args[1].as_number());

            if (port.empty() || baud <= 0) {
                std::cerr << "[serial] serial_open: invalid port or baud rate\n";
                return core::value::nil();
            }

            std::lock_guard<std::mutex> lock(mutex_);
            auto sp = std::make_unique<utilities::serial_port>();
            if (!sp->open(port, baud)) {
                std::cerr << "[serial] serial_open: " << sp->last_error() << "\n";
                return core::value::nil();
            }
            intptr_t h = next_handle_++;
            ports_.emplace(h, std::move(sp));
            return core::value::handle(h);
        }

        // ── serial_close(handle) ──────────────────────────────────────────
        core::value do_close(span<const core::value> args) {
            intptr_t h = args[0].as_handle();
            std::lock_guard<std::mutex> lock(mutex_);
            auto it = ports_.find(h);
            if (it == ports_.end()) {
                std::cerr << "[serial] serial_close: unknown handle\n";
                return core::value::trigger(false);
            }
            ports_.erase(it);
            return core::value::trigger(true);
        }

        // ── serial_write(handle, data: string) ────────────────────────────
        core::value do_write(span<const core::value> args) {
            intptr_t h = args[0].as_handle();
            std::string data = args[1].as_string();

            std::lock_guard<std::mutex> lock(mutex_);
            auto it = ports_.find(h);
            if (it == ports_.end() || !it->second || !it->second->is_open()) {
                std::cerr << "[serial] serial_write: invalid or closed handle\n";
                return core::value::trigger(false);
            }
            int written = it->second->write(data);
            return core::value::trigger(written >= 0);
        }
    };

} // namespace idyl::modules
