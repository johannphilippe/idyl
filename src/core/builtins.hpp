#pragma once 

#include <string>
#include <cmath>
#include <cstdlib>

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
        size_t n = fd.length();
        std::cout << "Flow with " << n << " elements:\n";
        for (size_t i = 0; i < n; ++i) {
            std::cout << "Element " << i << ":\n";
            for (const auto& member : fd.members_) {
                if (i < member.elements_.size()) {
                    std::cout << "  " << member.name_ << ": ";
                    const value& val = member.elements_[i];
                    switch (val.type_) {
                        case value_t::number:
                            std::cout << val.number_ << "\n";
                            break;
                        case value_t::trigger:
                            std::cout << (val.trigger_ ? "true" : "false") << "\n";
                            break;
                        case value_t::time:
                            std::cout << val.number_ << " ms\n";
                            break;
                        case value_t::string:
                            std::cout << (val.string_ ? *val.string_ : "") << "\n";
                            break;
                        case value_t::flow:
                            if (val.flow_) {
                                recursive_print_flow(*val.flow_);
                            } else {
                                std::cout << "null flow\n";
                            }
                            break;
                        case value_t::handle:
                            std::cout << val.as_string() << "\n";
                            break;
                        default:
                            std::cout << "nil or unknown type\n";
                    }
                } else {
                    std::cout << "  " << member.name_ << ": <no element>\n";
                }
            }
        }
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
                        default:
                            std::cout << "<nil>";
                    }
                }
                std::cout << std::endl;
                return value::nil();
            }, 1, -1
        },
    };
    constexpr size_t num_builtins = sizeof(builtins) / sizeof(builtin);
} // --- idyl::core ---