#pragma once

// =============================================================================
// midi_module.hpp — MIDI module for idyl (via RtMidi)
//
// Exposes MIDI I/O to idyl programs via opaque handles.
// Covers all major MIDI 1.0 channel messages.
//
// Functions:
//   midi_ports_in()                              → print + return flow of input port names
//   midi_ports_out()                             → print + return flow of output port names
//   handle = midi_out(port_number)               → open MIDI output port
//   handle = midi_in(port_number)                → open MIDI input port
//   midi_close(handle)                           → close port
//
//   midi_note_on(handle, ch, note, vel)          → send Note On
//   midi_note_off(handle, ch, note, vel)         → send Note Off
//   midi_cc(handle, ch, controller, value)       → send Control Change
//   midi_program(handle, ch, program)            → send Program Change
//   midi_pitch_bend(handle, ch, value)           → send Pitch Bend (-8192..8191)
//   midi_pressure(handle, ch, value)             → send Channel Pressure (0..127)
//   midi_aftertouch(handle, ch, note, value)     → send Polyphonic Aftertouch
//
//   msg = midi_recv(handle, dt=1ms)              → native temporal: polls input each tick
//     output: flow [type, channel, data1, data2]
//       type: 8=note_off, 9=note_on, 11=cc, 12=program, 13=pressure, 14=pitch_bend
//       channel: 1–16
//       data1/data2: message-specific (see below)
//     emits: note_on, note_off, cc, program, pitch_bend, pressure
//
// data1/data2 per type:
//   note_on/off:   data1=note(0-127),       data2=velocity(0-127)
//   cc:            data1=controller(0-127), data2=value(0-127)
//   program:       data1=program(0-127),    data2=0
//   pressure:      data1=pressure(0-127),   data2=0
//   pitch_bend:    data1=value(-8192..8191),data2=0
// =============================================================================

#include "include/module.hpp"

#ifdef IDYL_MODULE_MIDI
#include <RtMidi.h>
#endif

#include <memory>
#include <mutex>
#include <iostream>
#include <unordered_map>
#include <vector>
#include <cstdint>

namespace idyl::modules {

struct midi_module : module::base_module {

#ifdef IDYL_MODULE_MIDI

    // ── Connection types ───────────────────────────────────────────────────
    enum class port_dir { output, input };

    struct port_conn {
        port_dir dir_;
        std::unique_ptr<RtMidiOut> out_;
        std::unique_ptr<RtMidiIn>  in_;
    };

    // ── State ──────────────────────────────────────────────────────────────
    std::unordered_map<intptr_t, std::unique_ptr<port_conn>> ports_;
    intptr_t next_handle_ = 1;
    mutable std::mutex mutex_;

#endif

    // ── base_module interface ──────────────────────────────────────────────
    std::string name() const override { return "midi"; }

    bool available() const override {
        #ifdef IDYL_MODULE_MIDI
        return true;
        #else
        return false;
        #endif
    }

    std::string unavailable_reason() const override {
        return "MIDI module was disabled at build time (cmake -DIDYL_MODULE_MIDI=OFF)";
    }

    void cleanup() noexcept override {
        #ifdef IDYL_MODULE_MIDI
        std::lock_guard<std::mutex> lock(mutex_);
        ports_.clear();
        #endif
    }

    std::vector<module::function_entry> functions() override {
        using namespace module;

#ifndef IDYL_MODULE_MIDI
        return {};
#else

        // ── Port listing ──────────────────────────────────────────────────
        function_entry ports_in_entry;
        ports_in_entry.name_      = "midi_ports_in";
        ports_in_entry.fn_        = [this](span<const core::value>) { return do_ports(port_dir::input); };
        ports_in_entry.min_arity_ = 0;
        ports_in_entry.max_arity_ = 0;

        function_entry ports_out_entry;
        ports_out_entry.name_      = "midi_ports_out";
        ports_out_entry.fn_        = [this](span<const core::value>) { return do_ports(port_dir::output); };
        ports_out_entry.min_arity_ = 0;
        ports_out_entry.max_arity_ = 0;

        // ── Open / close ──────────────────────────────────────────────────
        function_entry out_entry;
        out_entry.name_      = "midi_out";
        out_entry.fn_        = [this](span<const core::value> args) { return do_open_out(args); };
        out_entry.min_arity_ = 1;
        out_entry.max_arity_ = 1;

        function_entry in_entry;
        in_entry.name_      = "midi_in";
        in_entry.fn_        = [this](span<const core::value> args) { return do_open_in(args); };
        in_entry.min_arity_ = 1;
        in_entry.max_arity_ = 1;

        function_entry close_entry;
        close_entry.name_      = "midi_close";
        close_entry.fn_        = [this](span<const core::value> args) { return do_close(args); };
        close_entry.min_arity_ = 1;
        close_entry.max_arity_ = 1;

        // ── Send messages ─────────────────────────────────────────────────
        function_entry note_on_entry;
        note_on_entry.name_      = "midi_note_on";
        note_on_entry.fn_        = [this](span<const core::value> args) { return do_note_on(args); };
        note_on_entry.min_arity_ = 4;
        note_on_entry.max_arity_ = 4;

        function_entry note_off_entry;
        note_off_entry.name_      = "midi_note_off";
        note_off_entry.fn_        = [this](span<const core::value> args) { return do_note_off(args); };
        note_off_entry.min_arity_ = 4;
        note_off_entry.max_arity_ = 4;

        function_entry cc_entry;
        cc_entry.name_      = "midi_cc";
        cc_entry.fn_        = [this](span<const core::value> args) { return do_cc(args); };
        cc_entry.min_arity_ = 4;
        cc_entry.max_arity_ = 4;

        function_entry program_entry;
        program_entry.name_      = "midi_program";
        program_entry.fn_        = [this](span<const core::value> args) { return do_program(args); };
        program_entry.min_arity_ = 3;
        program_entry.max_arity_ = 3;

        function_entry pitch_bend_entry;
        pitch_bend_entry.name_      = "midi_pitch_bend";
        pitch_bend_entry.fn_        = [this](span<const core::value> args) { return do_pitch_bend(args); };
        pitch_bend_entry.min_arity_ = 3;
        pitch_bend_entry.max_arity_ = 3;

        function_entry pressure_entry;
        pressure_entry.name_      = "midi_pressure";
        pressure_entry.fn_        = [this](span<const core::value> args) { return do_pressure(args); };
        pressure_entry.min_arity_ = 3;
        pressure_entry.max_arity_ = 3;

        function_entry aftertouch_entry;
        aftertouch_entry.name_      = "midi_aftertouch";
        aftertouch_entry.fn_        = [this](span<const core::value> args) { return do_aftertouch(args); };
        aftertouch_entry.min_arity_ = 4;
        aftertouch_entry.max_arity_ = 4;

        // ── midi_recv: native temporal — polls MIDI input each tick ───────
        // Output flow: [type, channel, data1, data2]
        //   type: 8=note_off, 9=note_on, 11=cc, 12=program, 13=pressure, 14=pitch_bend
        // Emitted signals: note_on, note_off, cc, program, pitch_bend, pressure
        function_entry recv_entry;
        recv_entry.name_              = "midi_recv";
        recv_entry.is_native_temporal_ = true;
        recv_entry.params_ = {
            required("handle"),
            with_default_ms("dt", 1.0),
        };

        recv_entry.native_init_ = [](
            const core::native_state_t& /*params*/,
            core::native_state_t& state)
        {
            state["last_msg"] = core::value::nil();
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
            if (h == 0) { output = core::value::nil(); return; }

            std::lock_guard<std::mutex> lock(mutex_);
            auto pit = ports_.find(h);
            if (pit == ports_.end() ||
                pit->second->dir_ != port_dir::input ||
                !pit->second->in_) {
                output = core::value::nil();
                return;
            }
            auto& in = *pit->second->in_;

            // Drain all pending messages; track which types arrived
            bool got_note_on = false, got_note_off = false, got_cc = false;
            bool got_program = false, got_pitch_bend = false, got_pressure = false;
            std::vector<unsigned char> last_bytes;

            std::vector<unsigned char> buf;
            while (true) {
                buf.clear();
                in.getMessage(&buf);
                if (buf.empty()) break;
                if (buf.size() < 1) continue;

                uint8_t status = buf[0] & 0xF0;
                switch (status) {
                    case 0x80: got_note_off   = true; break;
                    case 0x90: got_note_on    = true; break;
                    case 0xB0: got_cc         = true; break;
                    case 0xC0: got_program    = true; break;
                    case 0xD0: got_pressure   = true; break;
                    case 0xE0: got_pitch_bend = true; break;
                    default: break;
                }
                last_bytes = buf;
            }

            if (last_bytes.empty()) {
                output = state["last_msg"];
                return;
            }

            // Build output flow from last received message
            core::value flow_val = build_msg_flow(last_bytes);
            state["last_msg"] = flow_val;
            output = flow_val;

            if (got_note_on)    emitted["note_on"]    = core::value::trigger(true);
            if (got_note_off)   emitted["note_off"]   = core::value::trigger(true);
            if (got_cc)         emitted["cc"]         = core::value::trigger(true);
            if (got_program)    emitted["program"]    = core::value::trigger(true);
            if (got_pitch_bend) emitted["pitch_bend"] = core::value::trigger(true);
            if (got_pressure)   emitted["pressure"]   = core::value::trigger(true);
        };

        return {
            std::move(ports_in_entry),
            std::move(ports_out_entry),
            std::move(out_entry),
            std::move(in_entry),
            std::move(close_entry),
            std::move(note_on_entry),
            std::move(note_off_entry),
            std::move(cc_entry),
            std::move(program_entry),
            std::move(pitch_bend_entry),
            std::move(pressure_entry),
            std::move(aftertouch_entry),
            std::move(recv_entry),
        };
#endif
    }

#ifdef IDYL_MODULE_MIDI
private:

    // ── Build output flow from raw MIDI bytes ──────────────────────────────
    // Returns a flow with 4 elements: [type, channel, data1, data2]
    static core::value build_msg_flow(const std::vector<unsigned char>& b) {
        uint8_t status  = b[0];
        uint8_t type    = (status >> 4) & 0x0F;   // 8..14
        uint8_t channel = (status & 0x0F) + 1;     // 1..16
        uint8_t d1 = b.size() > 1 ? b[1] : 0;
        uint8_t d2 = b.size() > 2 ? b[2] : 0;

        double data1 = static_cast<double>(d1);
        double data2 = static_cast<double>(d2);

        // Pitch bend: combine LSB + MSB into signed -8192..8191
        if (type == 0xE) {
            int raw = static_cast<int>(d1) | (static_cast<int>(d2) << 7);
            data1 = static_cast<double>(raw - 8192);
            data2 = 0.0;
        }

        auto fd = std::make_shared<core::flow_data>();
        core::flow_member fm;
        fm.elements_.push_back(core::value::number(static_cast<double>(type)));
        fm.elements_.push_back(core::value::number(static_cast<double>(channel)));
        fm.elements_.push_back(core::value::number(data1));
        fm.elements_.push_back(core::value::number(data2));
        fd->members_.push_back(std::move(fm));

        core::value v;
        v.type_ = core::value_t::flow;
        v.flow_ = std::move(fd);
        return v;
    }

    // ── Port listing ───────────────────────────────────────────────────────
    core::value do_ports(port_dir dir) {
        try {
            unsigned int count = 0;
            std::vector<std::string> names;

            if (dir == port_dir::output) {
                RtMidiOut tmp;
                count = tmp.getPortCount();
                for (unsigned int i = 0; i < count; ++i)
                    names.push_back(tmp.getPortName(i));
                std::cerr << "[midi] Output ports (" << count << "):\n";
            } else {
                RtMidiIn tmp;
                count = tmp.getPortCount();
                for (unsigned int i = 0; i < count; ++i)
                    names.push_back(tmp.getPortName(i));
                std::cerr << "[midi] Input ports (" << count << "):\n";
            }

            auto fd = std::make_shared<core::flow_data>();
            core::flow_member fm;
            for (unsigned int i = 0; i < count; ++i) {
                std::cerr << "[midi]   " << i << ": " << names[i] << "\n";
                fm.elements_.push_back(core::value::string(names[i]));
            }
            fd->members_.push_back(std::move(fm));

            core::value v;
            v.type_ = core::value_t::flow;
            v.flow_ = std::move(fd);
            return v;
        } catch (const RtMidiError& e) {
            std::cerr << "[midi] midi_ports: " << e.getMessage() << "\n";
            return core::value::nil();
        }
    }

    // ── Open output port ───────────────────────────────────────────────────
    core::value do_open_out(span<const core::value> args) {
        unsigned int port = static_cast<unsigned int>(args[0].as_number());
        try {
            auto conn = std::make_unique<port_conn>();
            conn->dir_ = port_dir::output;
            conn->out_ = std::make_unique<RtMidiOut>();
            if (port >= conn->out_->getPortCount()) {
                std::cerr << "[midi] midi_out: port " << port << " does not exist\n";
                return core::value::nil();
            }
            conn->out_->openPort(port);
            std::lock_guard<std::mutex> lock(mutex_);
            intptr_t h = next_handle_++;
            ports_.emplace(h, std::move(conn));
            return core::value::handle(h);
        } catch (const RtMidiError& e) {
            std::cerr << "[midi] midi_out: " << e.getMessage() << "\n";
            return core::value::nil();
        }
    }

    // ── Open input port ────────────────────────────────────────────────────
    core::value do_open_in(span<const core::value> args) {
        unsigned int port = static_cast<unsigned int>(args[0].as_number());
        try {
            auto conn = std::make_unique<port_conn>();
            conn->dir_ = port_dir::input;
            conn->in_ = std::make_unique<RtMidiIn>();
            if (port >= conn->in_->getPortCount()) {
                std::cerr << "[midi] midi_in: port " << port << " does not exist\n";
                return core::value::nil();
            }
            // Ignore sysex, timing, and active sensing — keep it clean
            conn->in_->ignoreTypes(true, true, true);
            conn->in_->openPort(port);
            std::lock_guard<std::mutex> lock(mutex_);
            intptr_t h = next_handle_++;
            ports_.emplace(h, std::move(conn));
            return core::value::handle(h);
        } catch (const RtMidiError& e) {
            std::cerr << "[midi] midi_in: " << e.getMessage() << "\n";
            return core::value::nil();
        }
    }

    // ── Close port ─────────────────────────────────────────────────────────
    core::value do_close(span<const core::value> args) {
        intptr_t h = args[0].as_handle();
        std::lock_guard<std::mutex> lock(mutex_);
        auto it = ports_.find(h);
        if (it == ports_.end()) {
            std::cerr << "[midi] midi_close: unknown handle\n";
            return core::value::trigger(false);
        }
        ports_.erase(it);
        return core::value::trigger(true);
    }

    // ── Send helpers ───────────────────────────────────────────────────────
    RtMidiOut* get_out(intptr_t h) {
        auto it = ports_.find(h);
        if (it == ports_.end() ||
            it->second->dir_ != port_dir::output ||
            !it->second->out_) return nullptr;
        return it->second->out_.get();
    }

    core::value send_msg(RtMidiOut& out, std::vector<unsigned char> msg) {
        try {
            out.sendMessage(&msg);
            return core::value::trigger(true);
        } catch (const RtMidiError& e) {
            std::cerr << "[midi] send: " << e.getMessage() << "\n";
            return core::value::trigger(false);
        }
    }

    // ch: 1-16 from idyl → 0-15 for MIDI wire
    static uint8_t wire_ch(double ch) {
        return static_cast<uint8_t>(std::max(1.0, std::min(16.0, ch)) - 1.0);
    }
    static uint8_t clamp7(double v) {
        return static_cast<uint8_t>(std::max(0.0, std::min(127.0, v)));
    }

    core::value do_note_on(span<const core::value> args) {
        intptr_t h = args[0].as_handle();
        std::lock_guard<std::mutex> lock(mutex_);
        auto* out = get_out(h);
        if (!out) { std::cerr << "[midi] midi_note_on: bad handle\n"; return core::value::trigger(false); }
        uint8_t ch  = wire_ch(args[1].as_number());
        uint8_t note = clamp7(args[2].as_number());
        uint8_t vel  = clamp7(args[3].as_number());
        return send_msg(*out, {static_cast<uint8_t>(0x90 | ch), note, vel});
    }

    core::value do_note_off(span<const core::value> args) {
        intptr_t h = args[0].as_handle();
        std::lock_guard<std::mutex> lock(mutex_);
        auto* out = get_out(h);
        if (!out) { std::cerr << "[midi] midi_note_off: bad handle\n"; return core::value::trigger(false); }
        uint8_t ch   = wire_ch(args[1].as_number());
        uint8_t note = clamp7(args[2].as_number());
        uint8_t vel  = clamp7(args[3].as_number());
        return send_msg(*out, {static_cast<uint8_t>(0x80 | ch), note, vel});
    }

    core::value do_cc(span<const core::value> args) {
        intptr_t h = args[0].as_handle();
        std::lock_guard<std::mutex> lock(mutex_);
        auto* out = get_out(h);
        if (!out) { std::cerr << "[midi] midi_cc: bad handle\n"; return core::value::trigger(false); }
        uint8_t ch   = wire_ch(args[1].as_number());
        uint8_t ctrl = clamp7(args[2].as_number());
        uint8_t val  = clamp7(args[3].as_number());
        return send_msg(*out, {static_cast<uint8_t>(0xB0 | ch), ctrl, val});
    }

    core::value do_program(span<const core::value> args) {
        intptr_t h = args[0].as_handle();
        std::lock_guard<std::mutex> lock(mutex_);
        auto* out = get_out(h);
        if (!out) { std::cerr << "[midi] midi_program: bad handle\n"; return core::value::trigger(false); }
        uint8_t ch  = wire_ch(args[1].as_number());
        uint8_t pgm = clamp7(args[2].as_number());
        return send_msg(*out, {static_cast<uint8_t>(0xC0 | ch), pgm});
    }

    core::value do_pitch_bend(span<const core::value> args) {
        intptr_t h = args[0].as_handle();
        std::lock_guard<std::mutex> lock(mutex_);
        auto* out = get_out(h);
        if (!out) { std::cerr << "[midi] midi_pitch_bend: bad handle\n"; return core::value::trigger(false); }
        uint8_t ch = wire_ch(args[1].as_number());
        // idyl value: -8192..8191 → wire: 0..16383 (center 8192)
        int raw = static_cast<int>(args[2].as_number()) + 8192;
        raw = std::max(0, std::min(16383, raw));
        uint8_t lsb = static_cast<uint8_t>(raw & 0x7F);
        uint8_t msb = static_cast<uint8_t>((raw >> 7) & 0x7F);
        return send_msg(*out, {static_cast<uint8_t>(0xE0 | ch), lsb, msb});
    }

    core::value do_pressure(span<const core::value> args) {
        intptr_t h = args[0].as_handle();
        std::lock_guard<std::mutex> lock(mutex_);
        auto* out = get_out(h);
        if (!out) { std::cerr << "[midi] midi_pressure: bad handle\n"; return core::value::trigger(false); }
        uint8_t ch  = wire_ch(args[1].as_number());
        uint8_t val = clamp7(args[2].as_number());
        return send_msg(*out, {static_cast<uint8_t>(0xD0 | ch), val});
    }

    core::value do_aftertouch(span<const core::value> args) {
        intptr_t h = args[0].as_handle();
        std::lock_guard<std::mutex> lock(mutex_);
        auto* out = get_out(h);
        if (!out) { std::cerr << "[midi] midi_aftertouch: bad handle\n"; return core::value::trigger(false); }
        uint8_t ch   = wire_ch(args[1].as_number());
        uint8_t note = clamp7(args[2].as_number());
        uint8_t val  = clamp7(args[3].as_number());
        return send_msg(*out, {static_cast<uint8_t>(0xA0 | ch), note, val});
    }

#endif // IDYL_MODULE_MIDI
};

} // namespace idyl::modules
