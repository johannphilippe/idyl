#pragma once 

#include <iostream> 
#include <vector>
#include "core/core.hpp"
#include "utilities/span.hpp"

/*
    Header to include for building modules 
*/

namespace idyl::module
{
    // Expose the public API for building modules (currently just the check function, but may expand in the future)
    inline void check(bool condition, const std::string& message = "Assertion failed")
    {
        if (!condition) {
            std::cerr << "Module Assertion Error: " << message << "\n";
        }
    }
    struct function
    {
        std::string name_;
        int arity_ = 0;
        bool is_temporal_ = false; 
        bool has_dt_param_ = false; 
        using function_ptr = idyl::core::value(*)(span<const idyl::core::value>);
    };

    struct module {
        std::vector<function> functions_;
    };

    static module* ud_module = nullptr; // Pointer to the user-defined module instance, to be populated by the module implementation

    bool create_module(const module& mod)
    {
        if(ud_module != nullptr)
        {
            std::cerr << "Error: Module instance already exists. Multiple calls to create_module() are not allowed.\n";
            return false;
        }
        ud_module = new module(mod);
        return true;
    }

    inline const char** idyl_module_init(int* count)
    {
        // Example module with two functions "foo" and "bar". Replace with actual module implementation.
        static const char* symbols[] = {"foo", "bar"};
        *count = sizeof(symbols) / sizeof(symbols[0]);
        return symbols;
    }

    inline module get_module()
    {
        if(ud_module == nullptr)
        {
            std::cerr << "Error: Module instance not created. Please call create_module() with a valid module definition before calling get_module().\n";
            return module{};
        }
        return *ud_module;
    }
}