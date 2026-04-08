// csound_module.hpp — Idyl Csound integration module
//
// Provides functions to compile, start, and schedule Csound orchestras and voices.
// Uses the Csound 6 C++ API. See dev/TODO.md for spec.

#pragma once

#include <string>
#include <unordered_map>
#include <memory>
#include <vector>
#ifdef IDYL_MODULE_CSOUND
#include <csound/csound.h>
#include <csound/csound.hpp>
#endif
#include "include/module.hpp"

namespace idyl::module {

class csound_module : public base_module {
public:
    csound_module();
    ~csound_module() override;

    std::string name() const override { return "csound"; }
    std::vector<function_entry> functions() override;
    bool available() const override;
    std::string unavailable_reason() const override;

    // Csound API integration
    struct Orchestra {
        std::unique_ptr<Csound> cs;
        std::string orc_path;
        bool started = false;
    };
    // Handle management
    int compile(const std::string& orc_path);
    bool start(int handle);
    int sched(const span<core::value>& params);

private:
    int next_handle_ = 1;
    std::unordered_map<int, std::shared_ptr<Orchestra>> orchestras_;
};

// ---------------- Implementation ----------------

inline csound_module::csound_module() { }

inline csound_module::~csound_module() {
    for (auto &p : orchestras_) {
        try {
            if (p.second && p.second->cs) {
                try { p.second->cs->Stop(); } catch(...) {}
            }
        } catch (...) {}
    }
}

inline std::vector<module::function_entry> csound_module::functions() {
    return {
        { "cs_compile",
          [this](span<const core::value> args) -> core::value {
              if (args.size_ < 1) return core::value::nil();
              std::string path = args[0].as_string();
              int h = compile(path);
              return (h != 0) ? core::value::handle(h) : core::value::nil();
          }, 1, 1 },

        { "cs_start",
          [this](span<const core::value> args) -> core::value {
              if (args.size_ < 1) return core::value::trigger(false);
              int h = static_cast<int>(args[0].as_handle());
              return core::value::number(start(h) ? 0.0 : 1.0); 
          }, 1, 1 },

        { "cs_sched",
          [this](span<const core::value> args) -> core::value {
              if (args.size_ < 4) return core::value::nil();
              int inst = sched(args);
              return (inst != 0) ? core::value::handle(inst) : core::value::nil();
          }, 4, -1 },

        { "cs_close",
          [this](span<const core::value> args) -> core::value {
              if (args.size_ < 1) return core::value::trigger(false);
              int h = static_cast<int>(args[0].as_handle());
              std::lock_guard<std::mutex> lock(mutex_);
              auto it = orchestras_.find(h);
              if (it == orchestras_.end()) return core::value::trigger(false);
              orchestras_.erase(it);
              return core::value::trigger(true);
          }, 1, 1 }
    };
}

inline bool csound_module::available() const {
#ifdef IDYL_MODULE_CSOUND
    return true;
#else
    return false;
#endif
}

inline std::string csound_module::unavailable_reason() const {
#ifndef IDYL_MODULE_CSOUND
    return "Csound development headers not available at build time.";
#else
    return std::string();
#endif
}

inline int csound_module::compile(const std::string& orc_path) {
    try {
        auto orch = std::make_shared<Orchestra>();
        orch->cs = std::make_unique<Csound>();

        std::vector<char*> argv;
        std::string prog = "csound";
        argv.push_back(const_cast<char*>(prog.c_str()));
        argv.push_back(const_cast<char*>(orc_path.c_str()));
        int argc = static_cast<int>(argv.size());

        int ret = 1;
        try {
            // Try C++ wrapper compile (may be available)
            ret = orch->cs->Compile(argc, argv.data());
        } catch (...) {
            // Fall back to C API compile
            CSOUND* csapi = csoundCreate(nullptr);
            if (!csapi) return 0;
            ret = csoundCompile(csapi, argc, argv.data());
            if (ret == 0) {
                orch->cs.reset(new Csound(csapi));
            } else {
                csoundDestroy(csapi);
            }
        }

        if (ret != 0) {
            std::cerr << "[csound] compile failed for: " << orc_path << "\n";
            return 0;
        }

        orch->orc_path = orc_path;
        int h = next_handle_++;
        orchestras_.emplace(h, std::move(orch));
        return h;
    } catch (const std::exception& e) {
        std::cerr << "[csound] exception in compile: " << e.what() << "\n";
        return 0;
    }
}

inline bool csound_module::start(int handle) {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = orchestras_.find(handle);
    if (it == orchestras_.end()) return false;
    auto& orch = it->second;
    if (!orch || !orch->cs) return false;
    try {
        orch->cs->Start();
        orch->started = true;
        return true;
    } catch (const std::exception& e) {
        std::cerr << "[csound] start exception: " << e.what() << "\n";
        return false;
    }
}

inline int csound_module::sched(const span<core::value>& args) {
    std::lock_guard<std::mutex> lock(mutex_);
    // Expecting at least 4 args: handle, instr, start, dur, followed by params
    auto it = orchestras_.find(handle);
    if (it == orchestras_.end()) return 0;
    auto& orch = it->second;
    if (!orch || !orch->cs) return 0;

    try {
        std::string score;
        double start_s = start / 1000.0;
        double dur_s   = dur   / 1000.0;

        bool is_num = true;
        for (char c : instr) if (!(c >= '0' && c <= '9')) { is_num = false; break; }
        if (is_num) {
            score = "i" + instr + " " + std::to_string(start_s) + " " + std::to_string(dur_s);
        } else {
            score = std::string("i1 ") + std::to_string(start_s) + " " + std::to_string(dur_s);
        }

        for (double p : params) {
            score += " ";
            score += std::to_string(p);
        }

        CSOUND* csapi = orch->cs->GetCsound();
        if (csapi) {
            csoundScoreEvent(csapi, score.c_str());
        } else {
            try { orch->cs->ScoreEvent(score.c_str()); } catch (...) {}
        }

        static int next_inst = 1;
        int inst_handle = next_inst++;
        return inst_handle;
    } catch (const std::exception& e) {
        std::cerr << "[csound] sched exception: " << e.what() << "\n";
        return 0;
    }
}

} // namespace idyl::module
