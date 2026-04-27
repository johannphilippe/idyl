#pragma once

#include <memory>
#include <string>
#include <vector>
#include <iostream>
#include <iomanip>

namespace idyl::parser {

// ============================================================================
// Node type enum for semantic analysis dispatch
// ============================================================================
enum class node_t {
    // Literals
    number_literal, time_literal, trigger_literal, rest_literal, string_literal,
    // Identifiers
    identifier,
    // Operators
    binary_op, unary_op, ternary_op, memory_op,
    // Flow/generators
    generator_expr, flow_literal, repetition_marker, flow_member, flow_definition,
    // Calls/access
    argument, function_call, flow_access, module_access,
    // Blocks
    init_block, lambda_block, parameter,
    // Expressions (wrappers)
    literal_expr, identifier_expr, binary_op_expr, unary_op_expr, ternary_op_expr,
    memory_op_expr, generator_expr_node, flow_literal_expr, function_call_expr,
    flow_access_expr, module_access_expr, parenthesized_expr,
    // Statements
    expression_stmt, assignment, catch_block, at_block, on_block,
    function_definition, process_block_body, process_block,
    library_import, module_import, stop_statement, start_statement, sched_statement,
    // Special expression: self-stop inside temporal lambda block
    self_stop_expr,
    // Top-level
    program,
};

// Base node class
struct node {
    const node_t type_;
    int line_ = 0;
    int column_ = 0;
    
    node(node_t type) : type_(type) {}
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
    using node::node;
    virtual expr_ptr clone() const = 0;
};

struct statement : node {
    using node::node;
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
    
    parameter() : node(node_t::parameter) {}
    
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
    
    number_literal() : node(node_t::number_literal) {}
    
    void print(int indent = 0) const override {
        printIndent(indent);
        std::cout << "NumberLiteral(" << value_ << ")\n";
    }
};

struct time_literal : node {
    std::string value_;
    std::string unit_; // "ms", "s", "hz", "b"
    
    time_literal() : node(node_t::time_literal) {}
    
    void print(int indent = 0) const override {
        printIndent(indent);
        std::cout << "TimeLiteral(" << value_ << unit_ << ")\n";
    }
};

struct trigger_literal : node {
    trigger_literal() : node(node_t::trigger_literal) {}
    
    void print(int indent = 0) const override {
        printIndent(indent);
        std::cout << "TriggerLiteral(!)\n";
    }
};

struct rest_literal : node {
    rest_literal() : node(node_t::rest_literal) {}
    
    void print(int indent = 0) const override {
        printIndent(indent);
        std::cout << "RestLiteral(_)\n";
    }
};

struct string_literal : node {
    std::string value_;
    
    string_literal() : node(node_t::string_literal) {}
    
    void print(int indent = 0) const override {
        printIndent(indent);
        std::cout << "StringLiteral(\"" << value_ << "\")\n";
    }
};

struct identifier : node {
    std::string name_;
    bool is_trigger_param_ = false; // true if "name!", false if "name"
    
    identifier() : node(node_t::identifier) {}
    
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
    
    binary_op() : node(node_t::binary_op) {}
    
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
    
    unary_op() : node(node_t::unary_op) {}
    
    void print(int indent = 0) const override {
        printIndent(indent);
        std::cout << "UnaryOp(" << op_ << ")\n";
        if (operand_) operand_->print(indent + 1);
    }
};

struct ternary_op : node {
    std::vector<expr_ptr> options_; // All options separated by ;
    expr_ptr condition_;
    
    ternary_op() : node(node_t::ternary_op) {}
    
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
    
    memory_op() : node(node_t::memory_op) {}
    
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
    
    generator_expr() : node(node_t::generator_expr) {}
    
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
    
    flow_literal() : node(node_t::flow_literal) {}
    
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
    
    repetition_marker() : node(node_t::repetition_marker) {}
    
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
    std::string gate_name_; // non-empty → only advance cursor when this member's trigger is live
    expr_ptr value_; // generator_expr or flow_literal

    flow_member() : node(node_t::flow_member) {}

    void print(int indent = 0) const override {
        printIndent(indent);
        std::cout << "FlowMember(" << name_;
        if (!gate_name_.empty()) std::cout << " on " << gate_name_;
        std::cout << ")\n";
        if (value_) value_->print(indent + 1);
    }
};

struct flow_definition : statement {
    std::string name_;
    std::vector<param_ptr> parameters_;
    std::vector<std::shared_ptr<flow_member>> members_; // Single member flow or multi-member
    
    flow_definition() : statement(node_t::flow_definition) {}
    
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
    
    argument() : node(node_t::argument) {}
    
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
    
    function_call() : node(node_t::function_call) {}
    
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
    
    flow_access() : node(node_t::flow_access) {}
    
    void print(int indent = 0) const override {
        printIndent(indent);
        if (!member_.empty()) {
            std::cout << "FlowAccess(flow." << member_ << ")\n";
        } else {
            std::cout << "FlowAccess(flow[index])\n";
        }
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
    
    module_access() : node(node_t::module_access) {}
    
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
    
    init_block() : node(node_t::init_block) {}
    
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
    
    lambda_block() : node(node_t::lambda_block) {}
    
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
    
    literal_expr() : expression(node_t::literal_expr) {}
    
    void print(int indent = 0) const override {
        if (literal_) literal_->print(indent);
    }
    
    expr_ptr clone() const override {
        return std::make_shared<literal_expr>(*this);
    }
};

struct identifier_expr : expression {
    std::shared_ptr<idyl::parser::identifier> identifier_;
    
    identifier_expr() : expression(node_t::identifier_expr) {}
    
    void print(int indent = 0) const override {
        if (identifier_) identifier_->print(indent);
    }
    
    expr_ptr clone() const override {
        return std::make_shared<identifier_expr>(*this);
    }
};

struct binary_op_expr : expression {
    std::shared_ptr<binary_op> op_;
    
    binary_op_expr() : expression(node_t::binary_op_expr) {}
    
    void print(int indent = 0) const override {
        if (op_) op_->print(indent);
    }
    
    expr_ptr clone() const override {
        return std::make_shared<binary_op_expr>(*this);
    }
};

struct unary_op_expr : expression {
    std::shared_ptr<unary_op> op_;
    
    unary_op_expr() : expression(node_t::unary_op_expr) {}
    
    void print(int indent = 0) const override {
        if (op_) op_->print(indent);
    }
    
    expr_ptr clone() const override {
        return std::make_shared<unary_op_expr>(*this);
    }
};

struct ternary_op_expr : expression {
    std::shared_ptr<ternary_op> op_;
    
    ternary_op_expr() : expression(node_t::ternary_op_expr) {}
    
    void print(int indent = 0) const override {
        if (op_) op_->print(indent);
    }
    
    expr_ptr clone() const override {
        return std::make_shared<ternary_op_expr>(*this);
    }
};

struct memory_op_expr : expression {
    std::shared_ptr<memory_op> op_;

    memory_op_expr() : expression(node_t::memory_op_expr) {}
    
    void print(int indent = 0) const override {
        if (op_) op_->print(indent);
    }
    
    expr_ptr clone() const override {
        return std::make_shared<memory_op_expr>(*this);
    }
};

struct generator_expr_node : expression {
    std::shared_ptr<generator_expr> generator_;
    
    generator_expr_node() : expression(node_t::generator_expr_node) {}
    
    void print(int indent = 0) const override {
        if (generator_) generator_->print(indent);
    }
    
    expr_ptr clone() const override {
        return std::make_shared<generator_expr_node>(*this);
    }
};

struct flow_literal_expr : expression {
    std::shared_ptr<flow_literal> flow_;
    
    flow_literal_expr() : expression(node_t::flow_literal_expr) {}
    
    void print(int indent = 0) const override {
        if (flow_) flow_->print(indent);
    }
    
    expr_ptr clone() const override {
        return std::make_shared<flow_literal_expr>(*this);
    }
};

struct function_call_expr : expression {
    std::shared_ptr<function_call> call_;
    
    function_call_expr() : expression(node_t::function_call_expr) {}
    
    void print(int indent = 0) const override {
        if (call_) call_->print(indent);
    }
    
    expr_ptr clone() const override {
        return std::make_shared<function_call_expr>(*this);
    }
};

struct flow_access_expr : expression {
    std::shared_ptr<flow_access> access_;
    
    flow_access_expr() : expression(node_t::flow_access_expr) {}
    
    void print(int indent = 0) const override {
        if (access_) access_->print(indent);
    }
    
    expr_ptr clone() const override {
        return std::make_shared<flow_access_expr>(*this);
    }
};

struct module_access_expr : expression {
    std::shared_ptr<module_access> access_;
    
    module_access_expr() : expression(node_t::module_access_expr) {}
    
    void print(int indent = 0) const override {
        if (access_) access_->print(indent);
    }
    
    expr_ptr clone() const override {
        return std::make_shared<module_access_expr>(*this);
    }
};

struct parenthesized_expr : expression {
    expr_ptr expr_;
    
    parenthesized_expr() : expression(node_t::parenthesized_expr) {}
    
    void print(int indent = 0) const override {
        printIndent(indent);
        std::cout << "ParenthesizedExpr\n";
        if (expr_) expr_->print(indent + 1);
    }
    
    expr_ptr clone() const override {
        return std::make_shared<parenthesized_expr>(*this);
    }
};

// self_stop_expr: a bare `stop` used in expression context inside a temporal
// lambda block update section.  Throws SelfStopSignal when evaluated.
struct self_stop_expr : expression {
    self_stop_expr() : expression(node_t::self_stop_expr) {}

    void print(int indent = 0) const override {
        printIndent(indent);
        std::cout << "SelfStopExpr\n";
    }
    expr_ptr clone() const override {
        return std::make_shared<self_stop_expr>(*this);
    }
};

// ============================================================================
// Statements
// ============================================================================

struct expression_stmt : statement {
    expr_ptr expression_;
    
    expression_stmt() : statement(node_t::expression_stmt) {}
    
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
    
    assignment() : statement(node_t::assignment) {}
    
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
    expr_ptr    instance_expr_;      // the instance: identifier (named) or call expr (anonymous)
    std::string signal_name_;        // the emitted signal to watch, or "end"
    std::vector<stmt_ptr> handler_;
    bool        anonymous_instance_ = false; // true when instance_expr_ is not a plain identifier

    catch_block() : statement(node_t::catch_block) {}

    void print(int indent = 0) const override {
        printIndent(indent);
        std::cout << "CatchBlock(::" << signal_name_
                  << (anonymous_instance_ ? " [anonymous]" : "") << ")\n";
        if (instance_expr_) instance_expr_->print(indent + 1);
        for (const auto& stmt : handler_) {
            if (stmt) stmt->print(indent + 1);
        }
    }
};

struct at_block : statement {
    expr_ptr time_expr_;
    std::vector<stmt_ptr> handler_;
    
    at_block() : statement(node_t::at_block) {}
    
    void print(int indent = 0) const override {
        printIndent(indent);
        std::cout << "AtBlock(at ...)\n";
        if (time_expr_) time_expr_->print(indent + 1);
        for (const auto& stmt : handler_) {
            if (stmt) stmt->print(indent + 1);
        }
    }
};

struct on_block : statement {
    expr_ptr trigger_expr_;
    std::vector<stmt_ptr> handler_;

    on_block() : statement(node_t::on_block) {}

    void print(int indent = 0) const override {
        printIndent(indent);
        std::cout << "OnBlock\n";
        if (trigger_expr_) trigger_expr_->print(indent + 1);
        for (const auto& s : handler_)
            if (s) s->print(indent + 1);
    }
};

struct stop_statement : statement {
    // Empty = stop the current process.  Non-empty = stop the named process.
    std::string target_;
    stop_statement() : statement(node_t::stop_statement) {}

    void print(int indent = 0) const override {
        printIndent(indent);
        std::cout << "StopStatement(stop"
                  << (target_.empty() ? "" : " " + target_) << ")\n";
    }
};

struct start_statement : statement {
    std::string target_;
    start_statement() : statement(node_t::start_statement) {}

    void print(int indent = 0) const override {
        printIndent(indent);
        std::cout << "StartStatement(start"
                  << (target_.empty() ? "" : " " + target_) << ")\n";
    }
};

struct sched_statement : statement {
    std::string target_;   // name of the process to schedule
    sched_statement() : statement(node_t::sched_statement) {}

    void print(int indent = 0) const override {
        printIndent(indent);
        std::cout << "SchedStatement(sched " << target_ << ")\n";
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
    bool has_parens_ = false; // true when defined as name() = expr (callable, not a constant)

    function_definition() : statement(node_t::function_definition) {}
    
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
    
    process_block_body() : node(node_t::process_block_body) {}
    
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
    
    process_block() : statement(node_t::process_block) {}
    
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
    std::string namespace_; // Empty = merge into global scope
    std::string path_;
    
    library_import() : statement(node_t::library_import) {}
    
    void print(int indent = 0) const override {
        printIndent(indent);
        if (namespace_.empty()) {
            std::cout << "LibraryImport(\"" << path_ << "\")\n";
        } else {
            std::cout << "LibraryImport(" << namespace_ << " = import(\"" << path_ << "\"))\n";
        }
    }
};

struct module_import : statement {
    std::string namespace_; // Empty = register at global scope
    std::string path_;
    
    module_import() : statement(node_t::module_import) {}
    
    void print(int indent = 0) const override {
        printIndent(indent);
        if (namespace_.empty()) {
            std::cout << "ModuleImport(\"" << path_ << "\")\n";
        } else {
            std::cout << "ModuleImport(" << namespace_ << " = module(\"" << path_ << "\"))\n";
        }
    }
};

// ============================================================================
// Top-level Program
// ============================================================================

struct program : node {
    std::vector<stmt_ptr> statements_; // library_import, module_import, function_definition, flow_definition, process_block
    
    program() : node(node_t::program) {}
    
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
