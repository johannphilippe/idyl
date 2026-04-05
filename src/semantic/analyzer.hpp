#pragma once 

#include "parser/ast.hpp"
#include "semantic/symbol.hpp"
#include "semantic/scope.hpp"
#include <vector>
#include <set>
#include <string>

namespace idyl::semantic {

    enum class severity { error, warning, info };
    
    struct analyzer {
        struct diagnostic {
            severity level_ = severity::error;
            std::string message_;
            int line_;
            int column_;
        };

        void analyze(const parser::program& program);
        void print_analysis_results() const;
        bool has_errors() const;
        scope_stack scope_stack_;

        std::vector<analyzer::diagnostic> diagnostics_;
        std::vector< std::shared_ptr<idyl::parser::program> > libraries_;

        // Path to the main source file, used to resolve relative library paths
        std::string source_path_;

    private:
        // Set of already-loaded library paths (absolute) to prevent circular/duplicate imports
        std::set<std::string> loaded_libraries_;

        // Stack of files currently being processed (for circular import detection)
        std::vector<std::string> import_stack_;

        void global_scope_pass(const parser::program& program);

        // Recursively resolve an AST node, performing name resolution and type inference as needed. 
        void resolve(const parser::node_ptr& node);

        // Parse a library file and register its exported symbols.
        // Returns the parsed program AST, or nullptr on failure.
        std::shared_ptr<idyl::parser::program> load_library(const std::string& path, int import_line, int import_col);

        // Register every top-level symbol from a parsed library into the current scope,
        // optionally under a namespace prefix.
        void register_library_symbols(const parser::program& lib_program, const std::string& ns);

        // Check arity and parameter names for a function or flow call, and set inferred type if possible.
        void check_call(const parser::node_ptr& call_node, const symbol_info& callee_info);

        // Infer the type of an expression node from literals and known symbols.
        inferred_t infer_expr_type(const parser::node_ptr& node);

    };

} // --- idyl::semantic ---