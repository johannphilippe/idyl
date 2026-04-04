#pragma once 

#include "parser/ast.hpp"
#include "semantic/symbol.hpp"
#include "semantic/scope.hpp"
#include <vector>

namespace idyl::semantic {
    
    struct analyzer {
        struct error {
            std::string message_;
            int line_;
            int column_;
        };

        void analyze(const parser::program& program);
        void print_analysis_results() const;
        scope_stack scope_stack_;

        std::vector<analyzer::error> errors_;
    private:
        void global_scope_pass(const parser::program& program);
        void resolve(const parser::node_ptr& node); 
    };


}