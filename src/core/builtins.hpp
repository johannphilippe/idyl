#pragma once 

#include <string>
#include <cmath>
#include <cstdlib>
#include <ctime>

#include "utilities/math.hpp"
#include "utilities/span.hpp"
#include "utilities/safety.hpp"
#include "core/core.hpp"

namespace idyl::core {
    using builtin_fn = value(*)(span<const value>);

    struct builtin {
        std::string name_; 
        builtin_fn function_;
        int min_arity_ = 1;
        int max_arity_ = 1;
    };

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
                            std::cout << (val.string_ ? *val.string_ : "") << " ";
                            break;
                        case value_t::flow:
                            if (val.flow_) {
                                recursive_print_flow(*val.flow_);
                            } else {
                                std::cout << "null flow ";
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
                if (args[0].type_ == value_t::flow && args[0].flow_) {
                    return value::number(static_cast<double>(args[0].flow_->length()));
                }
                return value::number(0.0);
            }, 1, 1
        },
        // ── General utilities ───────────────────────────────────────────────────
        {
            "print", [](span<const value> args) -> value {
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
                            std::cout << (v.string_ ? *v.string_ : "");
                            break;
                        case value_t::flow:
                            if (v.flow_) {
                                recursive_print_flow(*v.flow_);
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
                        default:
                            std::cout << "<nil>";
                    }
                }
                std::cout << std::endl;
                return value::nil();
            }, 1, -1
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
                        if(args[0].string_ && *args[0].string_ == "ms") {
                            auto now = std::chrono::steady_clock::now();
                            auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();
                            return value::time_ms(static_cast<double>(ms)); 
                        } else if(args[0].string_ && *args[0].string_ == "s") {
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
    };
    constexpr size_t num_builtins = sizeof(builtins) / sizeof(builtin);
} // --- idyl::core ---