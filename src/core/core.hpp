#pragma once 

#include <variant>
#include <string> 
#include "semantic/symbol.hpp"


namespace idyl::core {
    struct value 
    {
        idyl::semantic::inferred_t type_;
        std::variant<double, std::string, bool> data_;
    };
} // --- idyl::core ---
