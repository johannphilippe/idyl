#pragma once 

#include <string>
#include <cmath>

#include "utilities/math.hpp"
#include "utilities/span.hpp"
#include "utilities/safety.hpp"
#include "semantic/symbol.hpp"
#include "core/core.hpp"

namespace idyl::core {
    using builtin_fn = value(*)(span<const value>);

    struct builtin {
        std::string name_; 
        builtin_fn function_;
    };

    const builtin builtins[] = {
        {
            "sin", [](span<const value> args) -> value {
                idyl::utilities::check(args.size_ == 1 && args[0].type_ == idyl::semantic::inferred_t::number);
                double arg = std::get<double>(args[0].data_);
                return value{ idyl::semantic::inferred_t::number, idyl::math::fast_sin(arg) };
            }
        },
        {
            "cos", [](span<const value> args) -> value {
                idyl::utilities::check(args.size_ == 1 && args[0].type_ == idyl::semantic::inferred_t::number);
                double arg = std::get<double>(args[0].data_);
                return value{ idyl::semantic::inferred_t::number, idyl::math::fast_cos(arg) };
            }
        },
        {
            "tan", [](span<const value> args) -> value {
                idyl::utilities::check(args.size_ == 1 && args[0].type_ == idyl::semantic::inferred_t::number);
                double arg = std::get<double>(args[0].data_);
                return value{ idyl::semantic::inferred_t::number, std::tan(arg) };
            }
        },
        {
            "log", [](span<const value> args) -> value {
                idyl::utilities::check(args.size_ == 1 && args[0].type_ == idyl::semantic::inferred_t::number);
                double arg = std::get<double>(args[0].data_);
                return value{ idyl::semantic::inferred_t::number, std::log(arg) };
            }
        },
        {
            "exp", [](span<const value> args) -> value {
                idyl::utilities::check(args.size_ == 1 && args[0].type_ == idyl::semantic::inferred_t::number);
                double arg = std::get<double>(args[0].data_);
                return value{ idyl::semantic::inferred_t::number, std::exp(arg) };
            }   
        }

    };
    constexpr size_t num_builtins = sizeof(builtins) / sizeof(builtin);
} // --- idyl::core ---