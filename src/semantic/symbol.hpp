#pragma once 

#include <string>
#include <vector> 

namespace idyl::semantic {

    enum class inferred_t {
        unknown,
        number,
        time,
        trigger,
        rest,
        flow, 
        function, 
        module,
    };

    enum class symbol_t {
        function, 
        flow, 
        parameter, 
        local_variable, 
        emit_variable, 
        builtin, 
        module, 
        library,
    };

    struct symbol_info {
        symbol_t type_; 
        std::string name_; 
        int line_ = 0;
        int column_ = 0;

        int arity_ = 0;
        int required_arity_ = 0;
        std::vector<std::string> param_names_; 
        std::vector<bool> param_has_default_; 

        bool is_temporal_ = false; 
        bool has_dt_param_ = false; 
        std::vector<std::string> trigger_params_; 

        inferred_t inferred_type_ = inferred_t::unknown;
    };

} // --- idyl::semantic ---
