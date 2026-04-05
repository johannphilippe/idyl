#include "semantic/analyzer.hpp"
#include "parser/ast.hpp"
#include <iostream>
#include <string>

namespace idyl {

    inline std::string resolve_node_t(parser::node_t type) {
        switch (type) {
            // Literals
            case parser::node_t::number_literal:     return "number_literal";
            case parser::node_t::time_literal:       return "time_literal";
            case parser::node_t::trigger_literal:    return "trigger_literal";
            case parser::node_t::rest_literal:       return "rest_literal";
            case parser::node_t::string_literal:     return "string_literal";
            // Identifiers
            case parser::node_t::identifier:         return "identifier";
            // Operators
            case parser::node_t::binary_op:          return "binary_op";
            case parser::node_t::unary_op:           return "unary_op";
            case parser::node_t::ternary_op:         return "ternary_op";
            case parser::node_t::memory_op:          return "memory_op";
            // Flow/generators
            case parser::node_t::generator_expr:     return "generator_expr";
            case parser::node_t::flow_literal:       return "flow_literal";
            case parser::node_t::repetition_marker:  return "repetition_marker";
            case parser::node_t::flow_member:        return "flow_member";
            case parser::node_t::flow_definition:    return "flow_definition";
            // Calls/access
            case parser::node_t::argument:           return "argument";
            case parser::node_t::function_call:      return "function_call";
            case parser::node_t::flow_access:        return "flow_access";
            case parser::node_t::module_access:      return "module_access";
            // Blocks
            case parser::node_t::init_block:         return "init_block";
            case parser::node_t::lambda_block:       return "lambda_block";
            case parser::node_t::parameter:          return "parameter";
            // Expressions (wrappers)
            case parser::node_t::literal_expr:       return "literal_expr";
            case parser::node_t::identifier_expr:    return "identifier_expr";
            case parser::node_t::binary_op_expr:     return "binary_op_expr";
            case parser::node_t::unary_op_expr:      return "unary_op_expr";
            case parser::node_t::ternary_op_expr:    return "ternary_op_expr";
            case parser::node_t::memory_op_expr:     return "memory_op_expr";
            case parser::node_t::generator_expr_node:return "generator_expr_node";
            case parser::node_t::flow_literal_expr:  return "flow_literal_expr";
            case parser::node_t::function_call_expr: return "function_call_expr";
            case parser::node_t::flow_access_expr:   return "flow_access_expr";
            case parser::node_t::module_access_expr: return "module_access_expr";
            case parser::node_t::parenthesized_expr: return "parenthesized_expr";
            // Statements
            case parser::node_t::expression_stmt:    return "expression_stmt";
            case parser::node_t::assignment:         return "assignment";
            case parser::node_t::catch_block:        return "catch_block";
            case parser::node_t::function_definition:return "function_definition";
            case parser::node_t::process_block_body: return "process_block_body";
            case parser::node_t::process_block:      return "process_block";
            case parser::node_t::library_import:     return "library_import";
            case parser::node_t::module_import:      return "module_import";
            // Top-level
            case parser::node_t::program:            return "program";
        }
        return "unknown";
    }

    inline std::string resolve_symbol_t(semantic::symbol_t type) {
        switch (type) {
            case semantic::symbol_t::function: return "function";
            case semantic::symbol_t::flow: return "flow";
            case semantic::symbol_t::flow_member: return "flow_member";
            case semantic::symbol_t::parameter: return "parameter";
            case semantic::symbol_t::local_variable: return "local_variable";
            case semantic::symbol_t::emit_variable: return "emit_variable";
            case semantic::symbol_t::builtin: return "builtin";
            case semantic::symbol_t::module: return "module";
            case semantic::symbol_t::library: return "library";
        }
        return "unknown";
    }

    inline std::string resolve_inferred_t(semantic::inferred_t type) {
        switch (type) {
            case semantic::inferred_t::unknown:  return "unknown";
            case semantic::inferred_t::number:   return "number";
            case semantic::inferred_t::time:     return "time";
            case semantic::inferred_t::trigger:  return "trigger";
            case semantic::inferred_t::flow:     return "flow";
            case semantic::inferred_t::function: return "function";
            case semantic::inferred_t::module:   return "module";
        }
        return "unknown";
    }

    inline void print_diagnostic(const idyl::semantic::analyzer::diagnostic& d) 
    {
        const char* label = "Error";
        if (d.level_ == idyl::semantic::severity::warning) label = "Warning";
        else if (d.level_ == idyl::semantic::severity::info) label = "Info";
        std::cout << "Semantic " << label << ": " << d.message_ << " at line " << d.line_ << ", column " << d.column_ << ".\n";
    }

    inline void debug(const std::string& message)
    {
        #ifdef IDYL_VERBOSE 
            std::cout << "Debug: " << message << "\n";
        #endif
    }

    inline void warning(const std::string& message, int line, int column)
    {
        std::cout << "Warning: " << message << " at line " << line << ", column " << column << ".\n";
    }

}