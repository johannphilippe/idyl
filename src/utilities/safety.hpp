#pragma once 

#include <iostream>

namespace idyl::utilities {
    
    inline void check(bool condition, const std::string& message = "Assertion failed")
    {
        if (!condition) {
            std::cerr << "Runtime Assertion Error: " << message << "\n";
        }
    }
}