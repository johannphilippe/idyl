#pragma once

#include <memory>
#include <string>
#include <vector>
#include <iostream>
#include <iomanip>

namespace idyl::parser {

// Base node class
struct node {
    int line_ = 0;
    int column_ = 0;
    
    virtual ~node() = default;
    virtual void print(int indent = 0) const = 0;
    
protected:
    void printIndent(int indent) const {
        std::cout << std::string(indent * 2, ' ');
    }
};

using node_ptr = std::shared_ptr<node>;

// ============================================================================
// Expressions (base class - defined early so it's available for use)
// ============================================================================
struct expression;
struct statement;
struct parameter;

using expr_ptr = std::shared_ptr<expression>;
using stmt_ptr = std::shared_ptr<statement>;
using param_ptr = std::shared_ptr<parameter>;

struct expression : node {
    virtual expr_ptr clone() const = 0;
};

struct statement : node {
    virtual ~statement() = default;
};

// ============================================================================
// Parameters
// ============================================================================

struct parameter : node {
    std::string name_;
    expr_ptr default_value_; // Optional
    bool is_trigger_parameter_ = false; // true for "name!"
    bool has_default_time_ = false; // true for "dt=value"
    
    void print(int indent = 0) const override {
        printIndent(indent);
        std::cout << "Parameter(" << name_;
        if (is_trigger_parameter_) std::cout << "!";
        if (default_value_) std::cout << " = ...";
        if (has_default_time_) std::cout << " (time param)";
        std::cout << ")\n";
        if (default_value_) default_value_->print(indent + 1);
    }
};

// ============================================================================
// Literals and Identifiers
// ============================================================================

struct number_literal : node {
    std::string value_;
    
    void print(int indent = 0) const override {
        printIndent(indent);
        std::cout << "NumberLiteral(" << value_ << ")\n";
    }
};

struct time_literal : node {
    std::string value_;
    std::string unit_; // "ms", "s", "hz", "b"
    
    void print(int indent = 0) const override {
        printIndent(indent);
        std::cout << "TimeLiteral(" << value_ << unit_ << ")\n";
    }
};

struct trigger_literal : node {
    void print(int indent = 0) const override {
        printIndent(indent);
        std::cout << "TriggerLiteral(!)\n";
    }
};

struct rest_literal : node {
    void print(int indent = 0) const override {
        printIndent(indent);
        std::cout << "RestLiteral(.)\n";
    }
};

struct string_literal : node {
    std::string value_;
    
    void print(int indent = 0) const override {
        printIndent(indent);
        std::cout << "StringLiteral(\"" << value_ << "\")\n";
    }
};

struct identifier : node {
    std::string name_;
    bool is_trigger_param_ = false; // true if "name!", false if "name"
    
    void print(int indent = 0) const override {
        printIndent(indent);
        if (is_trigger_param_) {
            std::cout << "Identifier(" << name_ << "!)\n";
        } else {
            std::cout << "Identifier(" << name_ << ")\n";
        }
    }
};

// ============================================================================
// Operators and Expressions
// ============================================================================

struct binary_op : node {
    std::string op_; // "+", "-", "*", "/", "%", "<", ">", "<=", ">=", "==", "!=", "&", "|", "^", "<<", ">>", "?"
    expr_ptr left_;
    expr_ptr right_;
    
    void print(int indent = 0) const override {
        printIndent(indent);
        std::cout << "BinaryOp(" << op_ << ")\n";
        if (left_) left_->print(indent + 1);
        if (right_) right_->print(indent + 1);
    }
};

struct unary_op : node {
    std::string op_; // "~", "-"
    expr_ptr operand_;
    
    void print(int indent = 0) const override {
        printIndent(indent);
        std::cout << "UnaryOp(" << op_ << ")\n";
        if (operand_) operand_->print(indent + 1);
    }
};

struct ternary_op : node {
    std::vector<expr_ptr> options_; // All options separated by ;
    expr_ptr condition_;
    
    void print(int indent = 0) const override {
        printIndent(indent);
        std::cout << "TernaryOp(; separated " << options_.size() << " options)\n";
        for (const auto& opt : options_) {
            if (opt) opt->print(indent + 1);
        }
        if (condition_) {
            printIndent(indent + 1);
            std::cout << "condition:\n";
            condition_->print(indent + 2);
        }
    }
};

struct memory_op : node {
    expr_ptr expr_;
    expr_ptr delay_count_; // nullptr for one-sample delay
    
    void print(int indent = 0) const override {
        printIndent(indent);
        std::cout << "MemoryOp(@)\n";
        if (expr_) expr_->print(indent + 1);
        if (delay_count_) {
            printIndent(indent + 1);
            std::cout << "delay:\n";
            delay_count_->print(indent + 2);
        }
    }
};

// ============================================================================
// Flow and Generator Expressions
// ============================================================================

struct generator_expr : node {
    std::string variable_;
    expr_ptr range_start_;
    expr_ptr range_end_;
    expr_ptr body_;
    
    void print(int indent = 0) const override {
        printIndent(indent);
        std::cout << "GeneratorExpr([" << variable_ << " = range : expr])\n";
        if (range_start_) range_start_->print(indent + 1);
        if (range_end_) range_end_->print(indent + 1);
        if (body_) body_->print(indent + 1);
    }
};

struct flow_literal : node {
    std::vector<expr_ptr> elements_;
    
    void print(int indent = 0) const override {
        printIndent(indent);
        std::cout << "FlowLiteral([" << elements_.size() << " elements])\n";
        for (const auto& elem : elements_) {
            if (elem) elem->print(indent + 1);
        }
    }
};

struct repetition_marker : node {
    expr_ptr repetition_count_; // nullptr for restart marker (||)
    
    void print(int indent = 0) const override {
        printIndent(indent);
        if (repetition_count_) {
            std::cout << "RepetitionMarker(|N|)\n";
            repetition_count_->print(indent + 1);
        } else {
            std::cout << "RestartMarker(||)\n";
        }
    }
};

struct flow_member : node {
    std::string name_;
    expr_ptr value_; // generator_expr or flow_literal
    
    void print(int indent = 0) const override {
        printIndent(indent);
        std::cout << "FlowMember(" << name_ << ")\n";
        if (value_) value_->print(indent + 1);
    }
};

struct flow_definition : statement {
    std::string name_;
    std::vector<param_ptr> parameters_;
    std::vector<std::shared_ptr<flow_member>> members_; // Single member flow or multi-member
    
    void print(int indent = 0) const override {
        printIndent(indent);
        std::cout << "FlowDefinition(" << name_ << ", " << parameters_.size() << " params, " << members_.size() << " members)\n";
        for (const auto& param : parameters_) {
            if (param) param->print(indent + 1);
        }
        for (const auto& member : members_) {
            if (member) member->print(indent + 1);
        }
    }
};

// ============================================================================
// Function Calls and Accesses
// ============================================================================

struct argument : node {
    std::string name_; // Empty for positional args
    expr_ptr value_;
    
    void print(int indent = 0) const override {
        printIndent(indent);
        if (!name_.empty()) {
            std::cout << "Argument(" << name_ << "=...)\n";
        } else {
            std::cout << "Argument(positional)\n";
        }
        if (value_) value_->print(indent + 1);
    }
};

struct function_call : node {
    expr_ptr function_;
    std::vector<std::shared_ptr<argument>> arguments_;
    
    void print(int indent = 0) const override {
        printIndent(indent);
        std::cout << "FunctionCall(" << arguments_.size() << " args)\n";
        if (function_) function_->print(indent + 1);
        for (const auto& arg : arguments_) {
            if (arg) arg->print(indent + 1);
        }
    }
};

struct flow_access : node {
    expr_ptr flow_;
    std::string member_;
    expr_ptr index_;
    
    void print(int indent = 0) const override {
        printIndent(indent);
        std::cout << "FlowAccess(flow::" << member_ << ")\n";
        if (flow_) flow_->print(indent + 1);
        if (index_) {
            printIndent(indent + 1);
            std::cout << "index:\n";
            index_->print(indent + 2);
        }
    }
};

struct module_access : node {
    expr_ptr module_;
    std::string function_;
    std::vector<std::shared_ptr<argument>> arguments_;
    
    void print(int indent = 0) const override {
        printIndent(indent);
        std::cout << "ModuleAccess(module::" << function_ << ", " << arguments_.size() << " args)\n";
        if (module_) module_->print(indent + 1);
        for (const auto& arg : arguments_) {
            if (arg) arg->print(indent + 1);
        }
    }
};

// ============================================================================
// Lambda Blocks and Temporal Functions
// ============================================================================

struct init_block : node {
    std::vector<stmt_ptr> statements_;
    
    void print(int indent = 0) const override {
        printIndent(indent);
        std::cout << "InitBlock(" << statements_.size() << " statements)\n";
        for (const auto& stmt : statements_) {
            if (stmt) stmt->print(indent + 1);
        }
    }
};

struct lambda_block : node {
    std::shared_ptr<init_block> init_; // Optional
    std::vector<stmt_ptr> update_statements_;
    
    void print(int indent = 0) const override {
        printIndent(indent);
        std::cout << "LambdaBlock(|> {...})\n";
        if (init_) init_->print(indent + 1);
        for (const auto& stmt : update_statements_) {
            if (stmt) stmt->print(indent + 1);
        }
    }
};

struct literal_expr : expression {
    node_ptr literal_; // number_literal, time_literal, trigger_literal, etc.
    
    void print(int indent = 0) const override {
        if (literal_) literal_->print(indent);
    }
    
    expr_ptr clone() const override {
        return std::make_shared<literal_expr>(*this);
    }
};

struct identifier_expr : expression {
    std::shared_ptr<idyl::parser::identifier> identifier_;
    
    void print(int indent = 0) const override {
        if (identifier_) identifier_->print(indent);
    }
    
    expr_ptr clone() const override {
        return std::make_shared<identifier_expr>(*this);
    }
};

struct binary_op_expr : expression {
    std::shared_ptr<binary_op> op_;
    
    void print(int indent = 0) const override {
        if (op_) op_->print(indent);
    }
    
    expr_ptr clone() const override {
        return std::make_shared<binary_op_expr>(*this);
    }
};

struct unary_op_expr : expression {
    std::shared_ptr<unary_op> op_;
    
    void print(int indent = 0) const override {
        if (op_) op_->print(indent);
    }
    
    expr_ptr clone() const override {
        return std::make_shared<unary_op_expr>(*this);
    }
};

struct ternary_op_expr : expression {
    std::shared_ptr<ternary_op> op_;
    
    void print(int indent = 0) const override {
        if (op_) op_->print(indent);
    }
    
    expr_ptr clone() const override {
        return std::make_shared<ternary_op_expr>(*this);
    }
};

struct memory_op_expr : expression {
    std::shared_ptr<memory_op> op_;
    
    void print(int indent = 0) const override {
        if (op_) op_->print(indent);
    }
    
    expr_ptr clone() const override {
        return std::make_shared<memory_op_expr>(*this);
    }
};

struct generator_expr_node : expression {
    std::shared_ptr<generator_expr> generator_;
    
    void print(int indent = 0) const override {
        if (generator_) generator_->print(indent);
    }
    
    expr_ptr clone() const override {
        return std::make_shared<generator_expr_node>(*this);
    }
};

struct flow_literal_expr : expression {
    std::shared_ptr<flow_literal> flow_;
    
    void print(int indent = 0) const override {
        if (flow_) flow_->print(indent);
    }
    
    expr_ptr clone() const override {
        return std::make_shared<flow_literal_expr>(*this);
    }
};

struct function_call_expr : expression {
    std::shared_ptr<function_call> call_;
    
    void print(int indent = 0) const override {
        if (call_) call_->print(indent);
    }
    
    expr_ptr clone() const override {
        return std::make_shared<function_call_expr>(*this);
    }
};

struct flow_access_expr : expression {
    std::shared_ptr<flow_access> access_;
    
    void print(int indent = 0) const override {
        if (access_) access_->print(indent);
    }
    
    expr_ptr clone() const override {
        return std::make_shared<flow_access_expr>(*this);
    }
};

struct module_access_expr : expression {
    std::shared_ptr<module_access> access_;
    
    void print(int indent = 0) const override {
        if (access_) access_->print(indent);
    }
    
    expr_ptr clone() const override {
        return std::make_shared<module_access_expr>(*this);
    }
};

struct parenthesized_expr : expression {
    expr_ptr expr_;
    
    void print(int indent = 0) const override {
        printIndent(indent);
        std::cout << "ParenthesizedExpr\n";
        if (expr_) expr_->print(indent + 1);
    }
    
    expr_ptr clone() const override {
        return std::make_shared<parenthesized_expr>(*this);
    }
};

// ============================================================================
// Statements
// ============================================================================

struct expression_stmt : statement {
    expr_ptr expression_;
    
    void print(int indent = 0) const override {
        printIndent(indent);
        std::cout << "ExpressionStmt\n";
        if (expression_) expression_->print(indent + 1);
    }
};

struct assignment : statement {
    std::string name_;
    expr_ptr value_;
    bool is_emit_ = false; // true for "emit name = value"
    
    void print(int indent = 0) const override {
        printIndent(indent);
        if (is_emit_) {
            std::cout << "Assignment(emit " << name_ << ")\n";
        } else {
            std::cout << "Assignment(" << name_ << ")\n";
        }
        if (value_) value_->print(indent + 1);
    }
};

struct catch_block : statement {
    expr_ptr expression_;
    std::string event_type_; // "end"
    std::vector<stmt_ptr> handler_;
    
    void print(int indent = 0) const override {
        printIndent(indent);
        std::cout << "CatchBlock(catch " << event_type_ << ")\n";
        if (expression_) expression_->print(indent + 1);
        for (const auto& stmt : handler_) {
            if (stmt) stmt->print(indent + 1);
        }
    }
};

// ============================================================================
// Function and Flow Definitions
// ============================================================================

struct function_definition : statement {
    std::string name_;
    std::vector<param_ptr> parameters_;
    expr_ptr body_;
    std::shared_ptr<lambda_block> lambda_block_; // Optional, for temporal functions
    
    void print(int indent = 0) const override {
        printIndent(indent);
        std::cout << "FunctionDefinition(" << name_ << ", " << parameters_.size() << " params)\n";
        for (const auto& param : parameters_) {
            if (param) param->print(indent + 1);
        }
        if (body_) body_->print(indent + 1);
        if (lambda_block_) lambda_block_->print(indent + 1);
    }
};

// ============================================================================
// Process Blocks
// ============================================================================

struct process_block_body : node {
    std::vector<stmt_ptr> statements_;
    
    void print(int indent = 0) const override {
        printIndent(indent);
        std::cout << "ProcessBlockBody(" << statements_.size() << " statements)\n";
        for (const auto& stmt : statements_) {
            if (stmt) stmt->print(indent + 1);
        }
    }
};

struct process_block : statement {
    std::string name_; // "main" by default, or custom name
    expr_ptr duration_; // Optional, for named process blocks
    std::shared_ptr<process_block_body> body_;
    
    void print(int indent = 0) const override {
        printIndent(indent);
        std::cout << "ProcessBlock(" << name_;
        if (duration_) std::cout << ", duration=...";
        std::cout << ")\n";
        if (body_) body_->print(indent + 1);
    }
};

// ============================================================================
// Module and Library Imports
// ============================================================================

struct library_import : statement {
    std::string path_;
    
    void print(int indent = 0) const override {
        printIndent(indent);
        std::cout << "LibraryImport(\"" << path_ << "\")\n";
    }
};

struct module_import : statement {
    std::string variable_name_;
    std::string module_path_;
    
    void print(int indent = 0) const override {
        printIndent(indent);
        std::cout << "ModuleImport(" << variable_name_ << " = import(\"" << module_path_ << "\"))\n";
    }
};

// ============================================================================
// Top-level Program
// ============================================================================

struct program : node {
    std::vector<stmt_ptr> statements_; // library_import, module_import, function_definition, flow_definition, process_block
    
    void print(int indent = 0) const override {
        printIndent(indent);
        std::cout << "Program(" << statements_.size() << " top-level statements)\n";
        for (const auto& stmt : statements_) {
            if (stmt) stmt->print(indent + 1);
        }
    }
};

// ============================================================================
// AST Printer
// ============================================================================

inline void print_ast(const program& program) {
    program.print();
}

} // namespace idyl::parser
