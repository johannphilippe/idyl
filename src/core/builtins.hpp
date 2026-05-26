#pragma once

#include <string>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <unordered_map>
#include <mutex>

#include "utilities/math.hpp"
#include "utilities/span.hpp"
#include "utilities/safety.hpp"
#include "utilities/filesystem.hpp"
#include "core/core.hpp"

namespace idyl::core {
    using builtin_fn = value(*)(span<const value>);

    struct builtin {
        std::string name_; 
        builtin_fn function_;
        int min_arity_ = 1;
        int max_arity_ = 1;
    };

    // Keeps shared_ptr<file_descriptor> alive while the handle is in use.
    inline auto& file_registry() {
        static std::unordered_map<intptr_t, std::shared_ptr<utilities::file_descriptor>> reg;
        return reg;
    }

    inline void recursive_print_flow(const flow_data& fd)
    {
        // Each member is printed in one line, with the member name and values. 
        size_t n = fd.length();
        std::cout << "Flow with " << n << " elements [ ";
        for (size_t i = 0; i < n; ++i) {
            for (const auto& member : fd.members_) {
                if (i < member.elements_.size()) {
                    const value& val = member.elements_[i];
                    switch (val.type_) {
                        case value_t::number:
                            std::cout << val.number_ << " ";
                            break;
                        case value_t::trigger:
                            std::cout << (val.trigger_ ? "true" : "false") << " ";
                            break;
                        case value_t::time:
                            std::cout << val.number_ << " ms ";
                            break;
                        case value_t::string:
                            std::cout << (val.payload_ ? val.str() : "") << " ";
                            break;
                        case value_t::flow:
                            if (val.payload_) {
                                recursive_print_flow(val.flow());
                            } else {
                                std::cout << "null flow ";
                            }
                            break;
                        case value_t::instance_ref:
                            if (val.payload_) {
                                value cur = val.inst().read_output();
                                std::cout << cur.as_string() << " ";
                            } else {
                                std::cout << "<null ref> ";
                            }
                            break;
                        case value_t::handle:
                            std::cout << val.as_string() << " ";
                            break;
                        default:
                            std::cout << "nil or unknown type ";
                    }
                } else {
                    std::cout << "  " << member.name_ << ": <no element>\n";
                }
            }
        }
        std::cout << "]\n";

    }

    const builtin builtins[] = {
        // ── Trigonometric ──────────────────────────────────────────────────────
        {
            "sin", [](span<const value> args) -> value {
                return value::number(idyl::math::fast_sin(args[0].as_number()));
            }, 1, 1
        },
        {
            "cos", [](span<const value> args) -> value {
                return value::number(idyl::math::fast_cos(args[0].as_number()));
            }, 1, 1
        },
        {
            "tan", [](span<const value> args) -> value {
                return value::number(std::tan(args[0].as_number()));
            }, 1, 1
        },
        // ── Exponential / logarithmic ──────────────────────────────────────────
        {
            "log", [](span<const value> args) -> value {
                return value::number(std::log(args[0].as_number()));
            }, 1, 1
        },
        {
            "exp", [](span<const value> args) -> value {
                return value::number(std::exp(args[0].as_number()));
            }, 1, 1
        },
        {
            "pow", [](span<const value> args) -> value {
                return value::number(std::pow(args[0].as_number(), args[1].as_number()));
            }, 2, 2
        },
        {
            "sqrt", [](span<const value> args) -> value {
                return value::number(std::sqrt(args[0].as_number()));
            }, 1, 1
        },
        // ── Arithmetic ─────────────────────────────────────────────────────────
        {
            "abs", [](span<const value> args) -> value {
                return value::number(std::abs(args[0].as_number()));
            }, 1, 1
        },
        {
            "floor", [](span<const value> args) -> value {
                return value::number(std::floor(args[0].as_number()));
            }, 1, 1
        },
        {
            "ceil", [](span<const value> args) -> value {
                return value::number(std::ceil(args[0].as_number()));
            }, 1, 1
        },
        {
            "frac", [](span<const value> args) -> value {
                double x = args[0].as_number();
                return value::number(x - std::floor(x));
            }, 1, 1
        },
        {
            "fmod", [](span<const value> args) -> value {
                return value::number(idyl::math::fmod(args[0].as_number(), args[1].as_number()));
            }, 2, 2
        },
        {
            "min", [](span<const value> args) -> value {
                return value::number(std::fmin(args[0].as_number(), args[1].as_number()));
            }, 2, 2
        },
        {
            "max", [](span<const value> args) -> value {
                return value::number(std::fmax(args[0].as_number(), args[1].as_number()));
            }, 2, 2
        },
        // ── Rounding / conversion ──────────────────────────────────────────────
        {
            "int", [](span<const value> args) -> value {
                return value::number(static_cast<double>(static_cast<int64_t>(args[0].as_number())));
            }, 1, 1
        },
        {
            "float", [](span<const value> args) -> value {
                return value::number(args[0].as_number());
            }, 1, 1
        },
        {
            "string", [](span<const value> args) -> value {
                return value::string(args[0].as_string());
            }, 1, 1
        },
        {
            "rint", [](span<const value> args) -> value {
                return value::number(std::rint(args[0].as_number()));
            }, 1, 1
        },
        // ── Bitwise ────────────────────────────────────────────────────────────
        {
            "bit", [](span<const value> args) -> value {
                int64_t val = static_cast<int64_t>(args[0].as_number());
                int64_t idx = static_cast<int64_t>(args[1].as_number());
                return value::number(static_cast<double>((val >> idx) & 1));
            }, 2, 2
        },
        // ── Random ─────────────────────────────────────────────────────────────
        {
            "rnd", [](span<const value> args) -> value {
                if (args.size_ == 0) {
                    return value::number(static_cast<double>(rand()) / RAND_MAX);
                } else if (args.size_ == 2) {
                    double lo = args[0].as_number();
                    double hi = args[1].as_number();
                    return value::number(lo + (static_cast<double>(rand()) / RAND_MAX) * (hi - lo));
                } else if (args.size_ == 3) {
                    double lo = args[0].as_number();
                    double hi = args[1].as_number();
                    double step = args[2].as_number();
                    double raw = lo + (static_cast<double>(rand()) / RAND_MAX) * (hi - lo);
                    if (step > 0.0) raw = std::round(raw / step) * step;
                    return value::number(raw);
                }
                return value::number(static_cast<double>(rand()) / RAND_MAX);
            }, 0, 3
        },
        {
            // integer random 
            "rndi", [](span<const value> args) -> value {
                if (args.size_ == 0) {
                    return value::number(rand() % 100); // default range [0, 100)
                } else if (args.size_ == 2) {
                    int64_t lo = static_cast<int64_t>(args[0].as_number());
                    int64_t hi = static_cast<int64_t>(args[1].as_number());
                    if (hi <= lo) return value::number(static_cast<double>(lo));
                    return value::number(lo + rand() % (hi - lo)); // range [lo, hi)
                } else if (args.size_ == 3) {
                    int64_t lo = static_cast<int64_t>(args[0].as_number());
                    int64_t hi = static_cast<int64_t>(args[1].as_number());
                    int64_t step = static_cast<int64_t>(args[2].as_number());
                    if (hi <= lo || step <= 0) return value::number(static_cast<double>(lo));
                    int64_t range = hi - lo;
                    int64_t steps = range / step;
                    return value::number(lo + (rand() % steps) * step); // range [lo, hi) with step
                }
                return value::number(rand() % 100);
            }, 0, 2
        },
        {
            "seed", [](span<const value> args) -> value {
                if (args.size_ > 0) {
                    unsigned int seed = static_cast<unsigned int>(args[0].as_number());
                    srand(seed);
                } else {
                    srand(static_cast<unsigned int>(::time(nullptr)));
                }
                return value::nil();
            }, 0, 1
        },
        // ── Flow utilities ─────────────────────────────────────────────────────
        {
            "len", [](span<const value> args) -> value {
                if (args[0].type_ == value_t::flow && args[0].payload_) {
                    return value::number(static_cast<double>(args[0].flow().length()));
                }
                return value::number(0.0);
            }, 1, 1
        },
        // ── General utilities (IO) ─────────────────────────────────────────────
        {
            "print", [](span<const value> args) -> value {
                // If there are any trigger-type args, at least one must be live.
                // This allows print(incr, reset) inside a lambda body to fire
                // when either trigger is active, while still suppressing output
                // when all triggers are at rest (e.g. print(res) where res=rest).
                bool has_trigger_arg = false;
                bool any_trigger_live = false;
                for (size_t i = 0; i < args.size_; ++i) {
                    const value& v = args[i];
                    if (v.type_ == value_t::trigger) {
                        has_trigger_arg = true;
                        if (v.trigger_) any_trigger_live = true;
                    }
                }
                if (has_trigger_arg && !any_trigger_live)
                    return value::nil();
                for (size_t i = 0; i < args.size_; ++i) {
                    if (i > 0) std::cout << " ";
                    const value& v = args[i];
                    switch (v.type_) {
                        case value_t::number:
                            std::cout << v.number_;
                            break;
                        case value_t::time:
                            std::cout << v.number_ << "ms";
                            break;
                        case value_t::trigger:
                            if(v.trigger_) std::cout << "trigger ! ";
                            else(std::cout << "rest _");
                            break;
                        case value_t::string:
                            std::cout << (v.payload_ ? v.str() : "");
                            break;
                        case value_t::flow:
                            if (v.payload_) {
                                recursive_print_flow(v.flow());
                            } else {
                                std::cout << "<null flow>";
                            }
                            break;
                        case value_t::handle:
                            std::cout << v.as_string();
                            break;
                        case value_t::function:
                            std::cout << v.as_string();
                            break;
                        case value_t::instance_ref:
                            if (v.payload_)
                                std::cout << v.inst().read_output().as_string();
                            else
                                std::cout << "<null ref>";
                            break;
                        default:
                            std::cout << "<nil>";
                    }
                }
                std::cout << std::endl;
                return value::nil();
            }, 1, -1
        },
        {
            "printf", [](span<const value> args) -> value {
                if (args.size_ == 0) return value::nil();
                std::string format = args[0].type_ == value_t::string && args[0].payload_ ? args[0].str() : "";
                size_t arg_index = 1;
                for (size_t i = 0; i < format.size(); ++i) {
                    if (format[i] == '%' && i + 1 < format.size()) {
                        char spec = format[++i];
                        if (arg_index >= args.size_) {
                            std::cerr << "Not enough arguments for printf format string\n";
                            break;
                        }
                        const value& v = args[arg_index++];
                        switch (spec) {
                            case 'd':
                            case 'i':
                                std::cout << static_cast<int64_t>(v.as_number());
                                break;
                            case 'f':
                                std::cout << v.as_number();
                                break;
                            case 's':
                                std::cout << v.as_string();
                                break;
                            case '%':
                                std::cout << '%';
                                break;
                            default:
                                std::cerr << "Unknown printf format specifier: %" << spec << "\n";
                        }
                    } else {
                        std::cout << format[i];
                    }
                }
                std::cout << std::endl;
                return value::nil();
            }, 1, -1
        },
        /*
            Open file, defaults to write mode 
            args: 
            - Path (string)
            - Mode (string, optional): "read"/"r", "write"/"w, "append"/"a". Default is "write".
        */
        { 
            "open", [](span<const value> args) -> value {
                if (args.size_ == 0 || args[0].type_ != value_t::string || !args[0].payload_) {
                    std::cerr << "Error: open() requires a string path argument.\n";
                    return value::nil();
                }
                utilities::file_descriptor::mode mode = utilities::file_descriptor::mode::write;
                if (args.size_ > 1 && args[1].type_ == value_t::string && args[1].payload_) {
                    std::string mode_str = args[1].str();
                    if (mode_str == "read" || mode_str == "r") {
                        mode = utilities::file_descriptor::mode::read;
                    } else if (mode_str == "write" || mode_str == "w") {
                        mode = utilities::file_descriptor::mode::write;
                    } else if (mode_str == "append" || mode_str == "a") {
                        mode = utilities::file_descriptor::mode::append;
                    }
                }
                std::string path = args[0].str();
                auto fd = std::make_shared<utilities::file_descriptor>(path, mode);
                if (!fd->is_open()) {
                    return value::nil();
                }
                intptr_t key = reinterpret_cast<intptr_t>(fd.get());
                file_registry()[key] = fd;
                return value::handle(key);
            }, 1, 2
        }, 
        {
            "close", [](span<const value> args) -> value {
                if (args.size_ < 1 || args[0].type_ != value_t::handle) {
                    std::cerr << "Error: close() requires a handle argument.\n";
                    return value::nil();
                }
                intptr_t key = args[0].handle_;
                auto& reg = file_registry();
                auto it = reg.find(key);
                if (it == reg.end()) {
                    std::cerr << "Error: Invalid or already-closed file handle.\n";
                    return value::nil();
                }
                it->second->close();
                reg.erase(it);
                return value::nil();
            }, 1, 1
        },
        /*
            Write string data to file
            args:
            - Handle (from open())
            - Data (string)

            Automatically writes a timestamp in ms since file open before the data, for easier debugging and temporal correlation of log entries.
        */
        {
            "write", [](span<const value> args) -> value {
                if (args.size_ < 2 || args[0].type_ != value_t::handle) {
                    std::cerr << "Error: write() requires a file handle and a value.\n";
                    return value::nil();
                }
                auto it = file_registry().find(args[0].handle_);
                if (it == file_registry().end()) {
                    std::cerr << "Error: Invalid or already-closed file handle.\n";
                    return value::nil();
                }
                it->second->write_timestamp();
                for(size_t i = 1; i < args.size_; ++i) {
                    it->second->write(args[i].as_string());
                }
                it->second->endl();
                return value::nil();
            }, 2, -1
        },
        /*
            Read file content as string
            args:
            - Handle (from open())
        */
        {
            "read", [](span<const value> args) -> value {
                if (args.size_ < 1 || args[0].type_ != value_t::handle) {
                    std::cerr << "Error: read() requires a file handle.\n";
                    return value::nil();
                }
                auto it = file_registry().find(args[0].handle_);
                if (it == file_registry().end()) {
                    std::cerr << "Error: Invalid or already-closed file handle.\n";
                    return value::nil();
                }
                return value::string(it->second->read());
            }, 1, 1
        },
        // ── Temporal utilities ───────────────────────────────────────────────────
        //  clock() and tempo() are handled as evaluator intrinsics
        //  (see evaluator.cpp eval_call) so they have access to the
        //  clock registry and named arguments.
        {
            "now", [](span<const value> args) -> value {
                if(args.size_ == 1) {
                    if(args[0].type_ == value_t::string)
                    {
                        if(args[0].payload_ && args[0].str() == "ms") {
                            auto now = std::chrono::steady_clock::now();
                            auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();
                            return value::time_ms(static_cast<double>(ms)); 
                        } else if(args[0].payload_ && args[0].str() == "s") {
                            auto now = std::chrono::steady_clock::now();
                            auto s = std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch()).count();
                            return value::time_ms(static_cast<double>(s * 1000)); 
                        }
                    }

                }
                auto now = std::chrono::steady_clock::now();
                auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();
                return value::time_ms(static_cast<double>(ms));
            }, 0, 1
        },
        // All temporal coercions art internally treated as ms 
        {
            "as_ms", [](span<const value> args) -> value {
                return value::time_ms(args[0].as_number());
            }, 1, 1
        },
        {
            "as_s", [](span<const value> args) -> value {
                return value::time_ms(args[0].as_number() * 1000.0);
            }, 1, 1
        },
        {
            "as_hz", [](span<const value> args) -> value {
                double ms = args[0].as_number();
                if (ms > 0.0) {
                    return value::number(1000.0 / ms);
                } else {
                    return value::number(0.0);
                }
            }, 1, 1
        },
        {
            "as_bpm", [](span<const value> args) -> value {
                double ms = args[0].as_number();
                if (ms > 0.0) {
                    return value::number(60000.0 / ms);
                } else {
                    return value::number(0.0);
                }
            }, 1, 1
        },
        {
            "trigger", [](span<const value> args) -> value {
                return value::trigger(args[0].as_number() != 0.0);
            }, 1, 1
        },
        // ── MIDI utilities ───────────────────────────────────────────────────
        {
            "mtof", [](span<const value> args) -> value {
                double m = args[0].as_number();
                double f = 440.0 * std::pow(2.0, (m - 69.0) / 12.0);
                return value::number(f);
            }, 1, 1
        },
        {
            "ftom", [](span<const value> args) -> value {
                double f = args[0].as_number();
                double m = 69.0 + 12.0 * std::log2(f / 440.0);
                return value::number(m);
            }, 1, 1
        }
    };
    constexpr size_t num_builtins = sizeof(builtins) / sizeof(builtin);
} // --- idyl::core ---