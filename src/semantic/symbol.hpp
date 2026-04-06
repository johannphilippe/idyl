#pragma once 

#include <string>
#include <vector> 

namespace idyl::semantic {

    enum class inferred_t {
        unknown,
        number,
        time,
        trigger,
        string,
        flow, 
        function, 
        module,
    };

    enum class symbol_t {
        function, 
        flow, 
        flow_member,
        parameter, 
        local_variable, 
        emit_variable, 
        builtin, 
        module, 
        library,
    };

    struct param_info {
        std::string name_;
        bool has_default_ = false;
        bool is_trigger_ = false;
        inferred_t expected_type_ = inferred_t::unknown;
    };

    struct symbol_info {
        symbol_t type_; 
        std::string name_; 
        int line_ = 0;
        int column_ = 0;

        int arity_ = 0;
        int required_arity_ = 0;

        std::vector<param_info> param_info_;

        bool is_temporal_ = false; 
        bool has_dt_param_ = false; 

        inferred_t inferred_type_ = inferred_t::unknown;

        // For flows: names of declared members (e.g. {kick: ..., snare: ...})
        std::vector<std::string> member_names_;

        // For unused-parameter detection: set to true when the symbol is referenced
        bool referenced_ = false;
    };

} // --- idyl::semantic ---
