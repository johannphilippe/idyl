%skeleton "glr2.cc"
%require "3.8"
%language "c++"

%expect 18

%locations
%define parse.error verbose
%define parse.trace
%define api.value.type variant
%define api.token.constructor
%lex-param {yy::lexer& driver}
%parse-param {yy::lexer& driver}

%code requires {
    #include <string>
    #include <vector>
    #include <memory>
    #include "ast.hpp"
    
    namespace yy {
        class lexer;
    }
}

%code {
    #include <iostream>
    #include <sstream>
    //#include "idyl_lexer.hpp"
    
    namespace yy {
        parser::symbol_type yylex(lexer& driver);
    }
    
    std::shared_ptr<idyl::parser::program> g_program = nullptr;
}

/* Tokens */
%token <std::string> IDENTIFIER
%token <std::string> NUMBER
%token <std::string> TIME_LITERAL
%token <std::string> STRING_LITERAL

%token FLOW PROCESS IMPORT MODULE INIT EMIT CATCH END DT DUR STOP START AGE PAUSE RESUME
%token LAMBDA_BLOCK NAMESPACE_DOT MEMORY_OP RANGE REST AT_OP
%token <std::string> REPEAT_MARKER
%token PLUS MINUS MUL DIV MOD
%token EQ NEQ LT GT LE GE
%token AND OR XOR NOT LSHIFT RSHIFT
%token QUESTION
%token ASSIGN COLON SEMICOLON COMMA DOT TRIGGER
%token LPAREN RPAREN LBRACKET RBRACKET LBRACE RBRACE
%token ON
%token EACH IN
%token YYEOF 0

/* Precedence and associativity */
%right ASSIGN
//%left OR
//%left AND
%left EQ NEQ
%left LT GT LE GE
%left PLUS MINUS
%left MUL DIV MOD
%left LSHIFT RSHIFT
%left AND OR XOR
%right NOT

%type <idyl::parser::node_ptr> program
%type <std::shared_ptr<idyl::parser::stop_statement>> stop_statement
%type <std::shared_ptr<idyl::parser::start_statement>> start_statement
%type <std::shared_ptr<idyl::parser::pause_statement>> pause_statement
%type <std::shared_ptr<idyl::parser::resume_statement>> resume_statement
%type <std::vector<idyl::parser::stmt_ptr>> top_level_statements
%type <idyl::parser::stmt_ptr> top_level_statement
%type <idyl::parser::stmt_ptr> function_or_flow_definition
%type <std::shared_ptr<idyl::parser::function_definition>> function_definition

%type <std::shared_ptr<idyl::parser::flow_definition>> flow_definition
%type <std::shared_ptr<idyl::parser::process_block>> process_block
%type <std::vector<idyl::parser::param_ptr>> parameter_list
%type <idyl::parser::param_ptr> parameter
%type <idyl::parser::expr_ptr> expression
%type <idyl::parser::expr_ptr> primary_expression
%type <idyl::parser::expr_ptr> postfix_expression
%type <idyl::parser::expr_ptr> unary_expression
%type <idyl::parser::expr_ptr> multiplicative_expression
%type <idyl::parser::expr_ptr> additive_expression
%type <idyl::parser::expr_ptr> shift_expression
%type <idyl::parser::expr_ptr> relational_expression
%type <idyl::parser::expr_ptr> equality_expression
%type <idyl::parser::expr_ptr> bitwise_and_expression
%type <idyl::parser::expr_ptr> bitwise_xor_expression
%type <idyl::parser::expr_ptr> bitwise_or_expression
%type <idyl::parser::expr_ptr> logical_and_expression
%type <idyl::parser::expr_ptr> logical_or_expression
%type <idyl::parser::expr_ptr> ternary_expression
%type <idyl::parser::expr_ptr> assignment_expression
%type <std::vector<idyl::parser::expr_ptr>> ternary_options
%type <std::vector<std::shared_ptr<idyl::parser::argument>>> argument_list
%type <std::shared_ptr<idyl::parser::argument>> argument
%type <std::shared_ptr<idyl::parser::lambda_block>> lambda_block
%type <std::shared_ptr<idyl::parser::init_block>> init_block
%type <std::vector<idyl::parser::stmt_ptr>> lambda_statements
%type <idyl::parser::stmt_ptr> lambda_statement
%type <std::vector<std::shared_ptr<idyl::parser::flow_member>>> flow_members
%type <idyl::parser::expr_ptr> flow_literal
%type <std::vector<idyl::parser::expr_ptr>> flow_elements
%type <idyl::parser::expr_ptr> generator_expression
%type <std::vector<idyl::parser::stmt_ptr>> process_body_statements
%type <idyl::parser::stmt_ptr> process_body_statement
%type <std::vector<idyl::parser::stmt_ptr>> block_body
%type <idyl::parser::stmt_ptr> block_body_statement
%type <std::shared_ptr<idyl::parser::catch_block>> catch_block
%type <std::shared_ptr<idyl::parser::at_block>> at_block
%type <std::shared_ptr<idyl::parser::on_block>> on_block
%type <std::shared_ptr<idyl::parser::each_block>> each_block
%type <std::vector<idyl::parser::stmt_ptr>> each_body
%type <idyl::parser::expr_ptr> each_dt_opt

%%

program
    : top_level_statements
    {
        auto prog = std::make_shared<idyl::parser::program>();
        prog->statements_ = $1;
        $$ = prog;
        g_program = prog;
    }
    | %empty
    {
        auto prog = std::make_shared<idyl::parser::program>();
        $$ = prog;
        g_program = prog;
    }
    ;

top_level_statements
    : top_level_statements top_level_statement
    {
        $$ = $1;
        if ($2) $$.push_back($2);
    }
    | top_level_statement
    {
        $$ = {};
        if ($1) $$.push_back($1);
    }
    ;

top_level_statement
    : function_or_flow_definition { $$ = $1; }
    | process_block { $$ = $1; }
    | IMPORT LPAREN STRING_LITERAL RPAREN
    {
        auto lib_import = std::make_shared<idyl::parser::library_import>();
        lib_import->path_ = $3;
        lib_import->line_ = @1.begin.line;
        lib_import->column_ = @1.begin.column;
        $$ = lib_import;
    }
    | IDENTIFIER ASSIGN IMPORT LPAREN STRING_LITERAL RPAREN
    {
        auto lib_import = std::make_shared<idyl::parser::library_import>();
        lib_import->namespace_ = $1;
        lib_import->path_ = $5;
        lib_import->line_ = @1.begin.line;
        lib_import->column_ = @1.begin.column;
        $$ = lib_import;
    }
    | MODULE LPAREN STRING_LITERAL RPAREN
    {
        auto mod_import = std::make_shared<idyl::parser::module_import>();
        mod_import->path_ = $3;
        mod_import->line_ = @1.begin.line;
        mod_import->column_ = @1.begin.column;
        $$ = mod_import;
    }
    | IDENTIFIER ASSIGN MODULE LPAREN STRING_LITERAL RPAREN
    {
        auto mod_import = std::make_shared<idyl::parser::module_import>();
        mod_import->namespace_ = $1;
        mod_import->path_ = $5;
        mod_import->line_ = @1.begin.line;
        mod_import->column_ = @1.begin.column;
        $$ = mod_import;
    }
    ;

function_or_flow_definition
    : function_definition { $$ = $1; }
    | flow_definition { $$ = $1; }
    ;

function_definition
    : IDENTIFIER LPAREN parameter_list RPAREN ASSIGN expression %prec ASSIGN
    {
        auto func = std::make_shared<idyl::parser::function_definition>();
        func->name_ = $1;
        func->parameters_ = $3;
        func->body_ = $6;
        func->line_ = @1.begin.line;
        func->column_ = @1.begin.column;
        $$ = func;
    }
    | IDENTIFIER LPAREN RPAREN ASSIGN expression %prec ASSIGN
    {
        auto func = std::make_shared<idyl::parser::function_definition>();
        func->name_ = $1;
        func->parameters_ = {};
        func->has_parens_ = true;
        func->body_ = $5;
        func->line_ = @1.begin.line;
        func->column_ = @1.begin.column;
        $$ = func;
    }
    | IDENTIFIER LPAREN parameter_list RPAREN ASSIGN expression lambda_block
    {
        auto func = std::make_shared<idyl::parser::function_definition>();
        func->name_ = $1;
        func->parameters_ = $3;
        func->body_ = $6;
        func->lambda_block_ = $7;
        func->line_ = @1.begin.line;
        func->column_ = @1.begin.column;
        $$ = func;
    }
    | IDENTIFIER LPAREN RPAREN ASSIGN expression lambda_block
    {
        auto func = std::make_shared<idyl::parser::function_definition>();
        func->name_ = $1;
        func->parameters_ = {};
        func->has_parens_ = true;
        func->body_ = $5;
        func->lambda_block_ = $6;
        func->line_ = @1.begin.line;
        func->column_ = @1.begin.column;
        $$ = func;
    }
    | IDENTIFIER ASSIGN expression %prec ASSIGN
    {
        auto func = std::make_shared<idyl::parser::function_definition>();
        func->name_ = $1;
        func->parameters_ = {};
        func->body_ = $3;
        func->line_ = @1.begin.line;
        func->column_ = @1.begin.column;
        $$ = func;
    }
    | IDENTIFIER ASSIGN expression lambda_block
    {
        auto func = std::make_shared<idyl::parser::function_definition>();
        func->name_ = $1;
        func->parameters_ = {};
        func->body_ = $3;
        func->lambda_block_ = $4;
        func->line_ = @1.begin.line;
        func->column_ = @1.begin.column;
        $$ = func;
    }
    ;

flow_definition
    : FLOW IDENTIFIER ASSIGN flow_literal
    {
        auto flow = std::make_shared<idyl::parser::flow_definition>();
        flow->name_ = $2;
        flow->parameters_ = {};
        auto member = std::make_shared<idyl::parser::flow_member>();
        member->name_ = "";
        member->value_ = $4;
        member->line_ = @3.begin.line;
        member->column_ = @3.begin.column;
        flow->members_.push_back(member);
        flow->line_ = @1.begin.line;
        flow->column_ = @1.begin.column;
        $$ = flow;
    }
    | FLOW IDENTIFIER LPAREN parameter_list RPAREN ASSIGN flow_literal
    {
        auto flow = std::make_shared<idyl::parser::flow_definition>();
        flow->name_ = $2;
        flow->parameters_ = $4;
        auto member = std::make_shared<idyl::parser::flow_member>();
        member->name_ = "";
        member->value_ = $7;
        member->line_ = @6.begin.line;
        member->column_ = @6.begin.column;
        flow->members_.push_back(member);
        flow->line_ = @1.begin.line;
        flow->column_ = @1.begin.column;
        $$ = flow;
    }
    | FLOW IDENTIFIER ASSIGN LBRACE flow_members RBRACE
    {
        auto flow = std::make_shared<idyl::parser::flow_definition>();
        flow->name_ = $2;
        flow->parameters_ = {};
        flow->members_ = $5;
        flow->line_ = @1.begin.line;
        flow->column_ = @1.begin.column;
        $$ = flow;
    }
    | FLOW IDENTIFIER LPAREN parameter_list RPAREN ASSIGN LBRACE flow_members RBRACE
    {
        auto flow = std::make_shared<idyl::parser::flow_definition>();
        flow->name_ = $2;
        flow->parameters_ = $4;
        flow->members_ = $8;
        flow->line_ = @1.begin.line;
        flow->column_ = @1.begin.column;
        $$ = flow;
    }
    ;

flow_members
    : flow_members IDENTIFIER COLON flow_literal
    {
        $$ = $1;
        auto member = std::make_shared<idyl::parser::flow_member>();
        member->name_ = $2;
        member->value_ = $4;
        member->line_ = @2.begin.line;
        member->column_ = @2.begin.column;
        $$.push_back(member);
    }
    | flow_members IDENTIFIER ON IDENTIFIER COLON flow_literal
    {
        $$ = $1;
        auto member = std::make_shared<idyl::parser::flow_member>();
        member->name_ = $2;
        member->gate_name_ = $4;
        member->value_ = $6;
        member->line_ = @2.begin.line;
        member->column_ = @2.begin.column;
        $$.push_back(member);
    }
    | IDENTIFIER COLON flow_literal
    {
        $$ = {};
        auto member = std::make_shared<idyl::parser::flow_member>();
        member->name_ = $1;
        member->value_ = $3;
        member->line_ = @1.begin.line;
        member->column_ = @1.begin.column;
        $$.push_back(member);
    }
    | IDENTIFIER ON IDENTIFIER COLON flow_literal
    {
        $$ = {};
        auto member = std::make_shared<idyl::parser::flow_member>();
        member->name_ = $1;
        member->gate_name_ = $3;
        member->value_ = $5;
        member->line_ = @1.begin.line;
        member->column_ = @1.begin.column;
        $$.push_back(member);
    }
    ;

flow_literal
    : LBRACKET flow_elements RBRACKET
    {
        auto flow_lit = std::make_shared<idyl::parser::flow_literal>();
        flow_lit->elements_ = $2;
        flow_lit->line_ = @1.begin.line;
        flow_lit->column_ = @1.begin.column;
        auto expr = std::make_shared<idyl::parser::flow_literal_expr>();
        expr->flow_ = flow_lit;
        expr->line_ = @1.begin.line;
        expr->column_ = @1.begin.column;
        $$ = expr;
    }
    | generator_expression { $$ = $1; }
    ;

flow_elements
    : flow_elements COMMA expression REPEAT_MARKER
    {
        $$ = $1;
        int n = std::stoi($4);
        if ($3 && $3->type_ == idyl::parser::node_t::flow_literal_expr) {
            // Group repeat: inline all inner elements N times (each with rc=1).
            auto& fle = static_cast<const idyl::parser::flow_literal_expr&>(*$3);
            if (fle.flow_)
                for (int i = 0; i < n; ++i)
                    for (const auto& e : fle.flow_->elements_)
                        $$.push_back(e);
        } else if ($3) {
            // Single-element repeat: wrap once with count=n.
            auto re = std::make_shared<idyl::parser::repeat_expr>();
            re->inner_ = $3;
            re->count_ = n;
            $$.push_back(re);
        }
    }
    | flow_elements COMMA expression
    {
        $$ = $1;
        $$.push_back($3);
    }
    | expression REPEAT_MARKER
    {
        int n = std::stoi($2);
        if ($1 && $1->type_ == idyl::parser::node_t::flow_literal_expr) {
            // Group repeat: inline all inner elements N times (each with rc=1).
            auto& fle = static_cast<const idyl::parser::flow_literal_expr&>(*$1);
            if (fle.flow_)
                for (int i = 0; i < n; ++i)
                    for (const auto& e : fle.flow_->elements_)
                        $$.push_back(e);
        } else if ($1) {
            // Single-element repeat: wrap once with count=n.
            auto re = std::make_shared<idyl::parser::repeat_expr>();
            re->inner_ = $1;
            re->count_ = n;
            $$.push_back(re);
        }
    }
    | expression { $$ = {$1}; }
    ;

generator_expression
    // count form: [i in N : body]
    : LBRACKET IDENTIFIER IN expression COLON expression RBRACKET
    {
        auto gen = std::make_shared<idyl::parser::generator_expr>();
        gen->variable_   = $2;
        gen->count_expr_ = $4;
        gen->body_       = $6;
        gen->line_       = @1.begin.line;
        gen->column_     = @1.begin.column;
        auto expr = std::make_shared<idyl::parser::generator_expr_node>();
        expr->generator_ = gen;
        expr->line_      = @1.begin.line;
        expr->column_    = @1.begin.column;
        $$ = expr;
    }
    // range form: [i in S..E : body]
    | LBRACKET IDENTIFIER IN expression RANGE expression COLON expression RBRACKET
    {
        auto gen = std::make_shared<idyl::parser::generator_expr>();
        gen->variable_    = $2;
        gen->range_start_ = $4;
        gen->range_end_   = $6;
        gen->body_        = $8;
        gen->line_        = @1.begin.line;
        gen->column_      = @1.begin.column;
        auto expr = std::make_shared<idyl::parser::generator_expr_node>();
        expr->generator_ = gen;
        expr->line_      = @1.begin.line;
        expr->column_    = @1.begin.column;
        $$ = expr;
    }
    // range+step form: [i in S..E..ST : body]
    | LBRACKET IDENTIFIER IN expression RANGE expression RANGE expression COLON expression RBRACKET
    {
        auto gen = std::make_shared<idyl::parser::generator_expr>();
        gen->variable_    = $2;
        gen->range_start_ = $4;
        gen->range_end_   = $6;
        gen->step_expr_   = $8;
        gen->body_        = $10;
        gen->line_        = @1.begin.line;
        gen->column_      = @1.begin.column;
        auto expr = std::make_shared<idyl::parser::generator_expr_node>();
        expr->generator_ = gen;
        expr->line_      = @1.begin.line;
        expr->column_    = @1.begin.column;
        $$ = expr;
    }
    ;

process_block
    : PROCESS COLON LBRACE process_body_statements RBRACE
    {
        auto proc = std::make_shared<idyl::parser::process_block>();
        proc->name_ = "main";
        auto body = std::make_shared<idyl::parser::process_block_body>();
        body->statements_ = $4;
        proc->body_ = body;
        proc->line_ = @1.begin.line;
        proc->column_ = @1.begin.column;
        $$ = proc;
    }
    | PROCESS IDENTIFIER COMMA DUR ASSIGN expression COLON LBRACE process_body_statements RBRACE
    {
        auto proc = std::make_shared<idyl::parser::process_block>();
        proc->name_ = $2;
        proc->duration_ = $6;
        auto body = std::make_shared<idyl::parser::process_block_body>();
        body->statements_ = $9;
        proc->body_ = body;
        proc->line_ = @1.begin.line;
        proc->column_ = @1.begin.column;
        $$ = proc;
    }
    | PROCESS IDENTIFIER COLON LBRACE process_body_statements RBRACE
    {
        auto proc = std::make_shared<idyl::parser::process_block>();
        proc->name_ = $2;
        auto body = std::make_shared<idyl::parser::process_block_body>();
        body->statements_ = $5;
        proc->body_ = body;
        proc->line_ = @1.begin.line;
        proc->column_ = @1.begin.column;
        $$ = proc;
    }
    ;

process_body_statements
    : process_body_statements process_body_statement
    {
        $$ = $1;
        if ($2) $$.push_back($2);
    }
    | process_body_statement
    {
        $$ = {};
        if ($1) $$.push_back($1);
    }
    ;

process_body_statement
    : postfix_expression ASSIGN expression %prec ASSIGN
    {
        // Interpret postfix_expression ASSIGN expression as a local function definition.
        // postfix_expression is either:
        //   - identifier_expr → 0-param binding: "name = expr" (same as assignment)
        //   - function_call_expr → local function: "name(params) = expr"
        auto func = std::make_shared<idyl::parser::function_definition>();
        func->body_ = $3;
        func->line_ = @1.begin.line;
        func->column_ = @1.begin.column;
        auto& lhs = $1;
        if (lhs && lhs->type_ == idyl::parser::node_t::function_call_expr) {
            // Extract function name from the call's function_ expression
            auto& call_expr = static_cast<idyl::parser::function_call_expr&>(*lhs);
            if (call_expr.call_ && call_expr.call_->function_) {
                auto& fn_expr = *call_expr.call_->function_;
                if (fn_expr.type_ == idyl::parser::node_t::identifier_expr) {
                    auto& id_expr = static_cast<idyl::parser::identifier_expr&>(fn_expr);
                    if (id_expr.identifier_) func->name_ = id_expr.identifier_->name_;
                }
            }
            // Convert arguments to parameters
            if (call_expr.call_) {
                for (const auto& arg : call_expr.call_->arguments_) {
                    if (!arg) continue;
                    auto param = std::make_shared<idyl::parser::parameter>();
                    if (!arg->name_.empty()) {
                        // Named arg: "dt=10ms" → parameter with default
                        param->name_ = arg->name_;
                        param->default_value_ = arg->value_;
                        param->has_default_time_ = (arg->name_ == "dt");
                    } else if (arg->value_ && arg->value_->type_ == idyl::parser::node_t::identifier_expr) {
                        auto& id = static_cast<idyl::parser::identifier_expr&>(*arg->value_);
                        if (id.identifier_) param->name_ = id.identifier_->name_;
                    } else if (arg->value_ && arg->value_->type_ == idyl::parser::node_t::unary_op_expr) {
                        // Could be trigger param: !name
                        param->name_ = "__trigger__";
                        param->is_trigger_parameter_ = true;
                    }
                    func->parameters_.push_back(param);
                }
            }
        } else if (lhs && lhs->type_ == idyl::parser::node_t::identifier_expr) {
            // Simple binding: "name = expr"
            auto& id_expr = static_cast<idyl::parser::identifier_expr&>(*lhs);
            if (id_expr.identifier_) func->name_ = id_expr.identifier_->name_;
        }
        $$ = func;
    }
    | IDENTIFIER ASSIGN expression
    {
        auto assign = std::make_shared<idyl::parser::assignment>();
        assign->name_ = $1;
        assign->value_ = $3;
        assign->is_emit_ = false;
        assign->line_ = @1.begin.line;
        assign->column_ = @1.begin.column;
        $$ = assign;
    }
    | EMIT IDENTIFIER ASSIGN expression
    {
        auto assign = std::make_shared<idyl::parser::assignment>();
        assign->name_ = $2;
        assign->value_ = $4;
        assign->is_emit_ = true;
        assign->line_ = @1.begin.line;
        assign->column_ = @1.begin.column;
        $$ = assign;
    }
    | stop_statement
    {
        $$ = $1;
    }
    | start_statement
    {
        $$ = $1;
    }
    | pause_statement
    {
        $$ = $1;
    }
    | resume_statement
    {
        $$ = $1;
    }
    | catch_block
    {
        $$ = $1;
    }
    | at_block
    {
        $$ = $1;
    }
    | on_block
    {
        $$ = $1;
    }
    | each_block
    {
        $$ = $1;
    }
    | expression
    {
        auto es = std::make_shared<idyl::parser::expression_stmt>();
        es->expression_ = $1;
        es->line_ = @1.begin.line;
        es->column_ = @1.begin.column;
        $$ = es;
    }
    ;

at_block
    : AT_OP LPAREN expression RPAREN COLON LBRACE process_body_statements RBRACE
    {
        auto at_stmt = std::make_shared<idyl::parser::at_block>();
        at_stmt->time_expr_ = $3;
        at_stmt->handler_ = $7;
        at_stmt->line_ = @1.begin.line;
        at_stmt->column_ = @1.begin.column;
        $$ = at_stmt;
    }
    ;

on_block
    : ON expression COLON LBRACE process_body_statements RBRACE
    {
        auto on_stmt = std::make_shared<idyl::parser::on_block>();
        on_stmt->trigger_expr_ = $2;
        on_stmt->handler_ = $5;
        on_stmt->line_ = @1.begin.line;
        on_stmt->column_ = @1.begin.column;
        $$ = on_stmt;
    }
    ;

// ── each_dt_opt: optional ", dt = expr" suffix ──────────────────────────────
each_dt_opt
    : %empty                         { $$ = nullptr; }
    | COMMA DT ASSIGN expression     { $$ = $4; }
    | COMMA expression               { $$ = $2; }  // positional dt
    ;

// ── each_body ────────────────────────────────────────────────────────────────
each_body
    : LBRACE process_body_statements RBRACE  { $$ = $2; }
    ;

// ── each_block ──────────────────────────────────────────────────────────────
// Forms:
//   each n in N [, dt=X] : body          simple count  (0 … N-1)
//   each n in S..E [, dt=X] : body       range         (S … E, step inferred)
//   each n in S..E..ST [, dt=X] : body   range + step
each_block
    // simple count: each n in N [dt] : body
    : EACH IDENTIFIER IN expression each_dt_opt COLON each_body
    {
        auto eb = std::make_shared<idyl::parser::each_block>();
        eb->var_name_   = $2;
        eb->count_expr_ = $4;
        eb->dt_expr_    = $5;
        eb->handler_    = $7;
        eb->line_       = @1.begin.line;
        eb->column_     = @1.begin.column;
        $$ = eb;
    }
    // range: each n in S..E [dt] : body
    | EACH IDENTIFIER IN expression RANGE expression each_dt_opt COLON each_body
    {
        auto eb = std::make_shared<idyl::parser::each_block>();
        eb->var_name_   = $2;
        eb->start_expr_ = $4;
        eb->end_expr_   = $6;
        eb->dt_expr_    = $7;
        eb->handler_    = $9;
        eb->line_       = @1.begin.line;
        eb->column_     = @1.begin.column;
        $$ = eb;
    }
    // range + step: each n in S..E..ST [dt] : body
    | EACH IDENTIFIER IN expression RANGE expression RANGE expression each_dt_opt COLON each_body
    {
        auto eb = std::make_shared<idyl::parser::each_block>();
        eb->var_name_   = $2;
        eb->start_expr_ = $4;
        eb->end_expr_   = $6;
        eb->step_expr_  = $8;
        eb->dt_expr_    = $9;
        eb->handler_    = $11;
        eb->line_       = @1.begin.line;
        eb->column_     = @1.begin.column;
        $$ = eb;
    }
    ;

stop_statement
    : STOP IDENTIFIER
    {
        auto stop_stmt = std::make_shared<idyl::parser::stop_statement>();
        stop_stmt->target_ = $2;
        stop_stmt->line_ = @1.begin.line;
        stop_stmt->column_ = @1.begin.column;
        $$ = stop_stmt;
    }
    ;

start_statement
    : START IDENTIFIER
    {
        auto start_stmt = std::make_shared<idyl::parser::start_statement>();
        start_stmt->target_ = $2;
        start_stmt->line_ = @1.begin.line;
        start_stmt->column_ = @1.begin.column;
        $$ = start_stmt;
    }
    ;

pause_statement
    : PAUSE IDENTIFIER
    {
        auto stmt = std::make_shared<idyl::parser::pause_statement>();
        stmt->target_ = $2;
        stmt->line_ = @1.begin.line;
        stmt->column_ = @1.begin.column;
        $$ = stmt;
    }
    | PAUSE
    {
        auto stmt = std::make_shared<idyl::parser::pause_statement>();
        stmt->target_ = "";   // empty = current process
        stmt->line_ = @1.begin.line;
        stmt->column_ = @1.begin.column;
        $$ = stmt;
    }
    ;

resume_statement
    : RESUME IDENTIFIER
    {
        auto stmt = std::make_shared<idyl::parser::resume_statement>();
        stmt->target_ = $2;
        stmt->line_ = @1.begin.line;
        stmt->column_ = @1.begin.column;
        $$ = stmt;
    }
    | RESUME
    {
        auto stmt = std::make_shared<idyl::parser::resume_statement>();
        stmt->target_ = "";   // empty = current process
        stmt->line_ = @1.begin.line;
        stmt->column_ = @1.begin.column;
        $$ = stmt;
    }
    ;

catch_block
    : CATCH postfix_expression NAMESPACE_DOT IDENTIFIER COLON LBRACE process_body_statements RBRACE
    {
        auto catch_b = std::make_shared<idyl::parser::catch_block>();
        catch_b->instance_expr_ = $2;
        catch_b->signal_name_   = $4;
        catch_b->handler_       = $7;
        catch_b->line_          = @1.begin.line;
        catch_b->column_        = @1.begin.column;
        $$ = catch_b;
    }
    | CATCH postfix_expression NAMESPACE_DOT END COLON LBRACE process_body_statements RBRACE
    {
        auto catch_b = std::make_shared<idyl::parser::catch_block>();
        catch_b->instance_expr_ = $2;
        catch_b->signal_name_   = "end";
        catch_b->handler_       = $7;
        catch_b->line_          = @1.begin.line;
        catch_b->column_        = @1.begin.column;
        $$ = catch_b;
    }
    ;

lambda_block
    : LAMBDA_BLOCK LBRACE init_block lambda_statements RBRACE
    {
        auto lambda = std::make_shared<idyl::parser::lambda_block>();
        lambda->init_ = $3;
        lambda->update_statements_ = $4;
        lambda->line_ = @1.begin.line;
        lambda->column_ = @1.begin.column;
        $$ = lambda;
    }
    | LAMBDA_BLOCK LBRACE init_block RBRACE
    {
        auto lambda = std::make_shared<idyl::parser::lambda_block>();
        lambda->init_ = $3;
        lambda->update_statements_ = {};
        lambda->line_ = @1.begin.line;
        lambda->column_ = @1.begin.column;
        $$ = lambda;
    }
    | LAMBDA_BLOCK LBRACE lambda_statements RBRACE
    {
        auto lambda = std::make_shared<idyl::parser::lambda_block>();
        lambda->update_statements_ = $3;
        lambda->line_ = @1.begin.line;
        lambda->column_ = @1.begin.column;
        $$ = lambda;
    }
    ;

init_block
    : INIT COLON LBRACE lambda_statements RBRACE
    {
        auto init = std::make_shared<idyl::parser::init_block>();
        init->statements_ = $4;
        init->line_ = @1.begin.line;
        init->column_ = @1.begin.column;
        $$ = init;
    }
    ;

lambda_statements
    : lambda_statements lambda_statement
    {
        $$ = $1;
        if ($2) $$.push_back($2);
    }
    | lambda_statement
    {
        $$ = {};
        if ($1) $$.push_back($1);
    }
    ;

lambda_statement
    : postfix_expression ASSIGN expression %prec ASSIGN
    {
        // Same pattern as in process_body_statement: interpret as local function definition
        // when lhs is a function call, or as an assignment when lhs is an identifier.
        auto func = std::make_shared<idyl::parser::function_definition>();
        func->body_ = $3;
        func->line_ = @1.begin.line;
        func->column_ = @1.begin.column;
        auto& lhs = $1;
        if (lhs && lhs->type_ == idyl::parser::node_t::function_call_expr) {
            auto& call_expr = static_cast<idyl::parser::function_call_expr&>(*lhs);
            if (call_expr.call_ && call_expr.call_->function_) {
                auto& fn_expr = *call_expr.call_->function_;
                if (fn_expr.type_ == idyl::parser::node_t::identifier_expr) {
                    auto& id_expr = static_cast<idyl::parser::identifier_expr&>(fn_expr);
                    if (id_expr.identifier_) func->name_ = id_expr.identifier_->name_;
                }
            }
            if (call_expr.call_) {
                for (const auto& arg : call_expr.call_->arguments_) {
                    if (!arg) continue;
                    auto param = std::make_shared<idyl::parser::parameter>();
                    if (!arg->name_.empty()) {
                        param->name_ = arg->name_;
                        param->default_value_ = arg->value_;
                        param->has_default_time_ = (arg->name_ == "dt");
                    } else if (arg->value_ && arg->value_->type_ == idyl::parser::node_t::identifier_expr) {
                        auto& id = static_cast<idyl::parser::identifier_expr&>(*arg->value_);
                        if (id.identifier_) param->name_ = id.identifier_->name_;
                    }
                    func->parameters_.push_back(param);
                }
            }
        } else if (lhs && lhs->type_ == idyl::parser::node_t::identifier_expr) {
            auto& id_expr = static_cast<idyl::parser::identifier_expr&>(*lhs);
            if (id_expr.identifier_) func->name_ = id_expr.identifier_->name_;
        }
        $$ = func;
    }
    | IDENTIFIER ASSIGN expression
    {
        auto assign = std::make_shared<idyl::parser::assignment>();
        assign->name_ = $1;
        assign->value_ = $3;
        assign->is_emit_ = false;
        assign->line_ = @1.begin.line;
        assign->column_ = @1.begin.column;
        $$ = assign;
    }
    | EMIT IDENTIFIER ASSIGN expression
    {
        auto assign = std::make_shared<idyl::parser::assignment>();
        assign->name_ = $2;
        assign->value_ = $4;
        assign->is_emit_ = true;
        assign->line_ = @1.begin.line;
        assign->column_ = @1.begin.column;
        $$ = assign;
    }
    | DT ASSIGN expression 
    {
        auto assign = std::make_shared<idyl::parser::assignment>();
        assign->name_ = "dt";
        assign->value_ = $3;
        assign->is_emit_ = false;
        assign->line_ = @1.begin.line;
        assign->column_ = @1.begin.column;
        $$ = assign;
    }
    | expression
    {
        auto es = std::make_shared<idyl::parser::expression_stmt>();
        es->expression_ = $1;
        es->line_ = @1.begin.line;
        es->column_ = @1.begin.column;
        $$ = es;
    }
    ;

block_body
    : block_body SEMICOLON block_body_statement
    {
        $$ = $1;
        if ($3) $$.push_back($3);
    }
    | block_body_statement
    {
        $$ = {};
        if ($1) $$.push_back($1);
    }
    | /* empty */ { $$ = {}; }
    ;

block_body_statement
    : IDENTIFIER ASSIGN expression
    {
        auto assign = std::make_shared<idyl::parser::assignment>();
        assign->name_ = $1;
        assign->value_ = $3;
        assign->is_emit_ = false;
        assign->line_ = @1.begin.line;
        assign->column_ = @1.begin.column;
        $$ = assign;
    }
    | EMIT IDENTIFIER ASSIGN expression
    {
        auto assign = std::make_shared<idyl::parser::assignment>();
        assign->name_ = $2;
        assign->value_ = $4;
        assign->is_emit_ = true;
        assign->line_ = @1.begin.line;
        assign->column_ = @1.begin.column;
        $$ = assign;
    }
    | stop_statement   { $$ = $1; }
    | start_statement  { $$ = $1; }
    | pause_statement  { $$ = $1; }
    | resume_statement { $$ = $1; }
    | expression
    {
        auto es = std::make_shared<idyl::parser::expression_stmt>();
        es->expression_ = $1;
        es->line_ = @1.begin.line;
        es->column_ = @1.begin.column;
        $$ = es;
    }
    ;

parameter_list
    : parameter_list COMMA parameter
    {
        $$ = $1;
        if ($3) $$.push_back($3);
    }
    | parameter { $$ = {$1}; }
    ;

parameter
    : IDENTIFIER
    {
        auto param = std::make_shared<idyl::parser::parameter>();
        param->name_ = $1;
        param->is_trigger_parameter_ = false;
        param->line_ = @1.begin.line;
        param->column_ = @1.begin.column;
        $$ = param;
    }
    | IDENTIFIER TRIGGER
    {
        auto param = std::make_shared<idyl::parser::parameter>();
        param->name_ = $1;
        param->is_trigger_parameter_ = true;
        param->line_ = @1.begin.line;
        param->column_ = @1.begin.column;
        $$ = param;
    }
    | IDENTIFIER ASSIGN expression
    {
        auto param = std::make_shared<idyl::parser::parameter>();
        param->name_ = $1;
        param->default_value_ = $3;
        param->line_ = @1.begin.line;
        param->column_ = @1.begin.column;
        $$ = param;
    }
    | DT ASSIGN expression
    {
        auto param = std::make_shared<idyl::parser::parameter>();
        param->name_ = "dt";
        param->default_value_ = $3;
        param->has_default_time_ = true;
        param->line_ = @1.begin.line;
        param->column_ = @1.begin.column;
        $$ = param;
    }
    ;

expression
    : assignment_expression { $$ = $1; }
    ;

assignment_expression
    : ternary_expression { $$ = $1; }
    ;

ternary_expression
    : logical_or_expression { $$ = $1; }
    | logical_or_expression QUESTION ternary_options
    {
        auto ternary = std::make_shared<idyl::parser::ternary_op>();
        ternary->options_ = $3;
        // Single-option form: cond ? expr  →  cond ? _ ; expr
        // options_[0] = false/rest branch, options_[1] = true branch
        if (ternary->options_.size() == 1) {
            auto rest = std::make_shared<idyl::parser::rest_literal>();
            rest->line_ = @2.begin.line;
            rest->column_ = @2.begin.column;
            auto rest_expr = std::make_shared<idyl::parser::literal_expr>();
            rest_expr->literal_ = rest;
            rest_expr->line_ = @2.begin.line;
            rest_expr->column_ = @2.begin.column;
            ternary->options_.insert(ternary->options_.begin(), rest_expr);
        }
        ternary->condition_ = $1;
        ternary->line_ = @2.begin.line;
        ternary->column_ = @2.begin.column;
        auto expr = std::make_shared<idyl::parser::ternary_op_expr>();
        expr->op_ = ternary;
        expr->line_ = @2.begin.line;
        expr->column_ = @2.begin.column;
        $$ = expr;
    }
    ;

ternary_options
    : ternary_options SEMICOLON logical_or_expression
    {
        $$ = $1;
        $$.push_back($3);
    }
    | logical_or_expression
    {
        $$ = {$1};
    }
    ;

logical_or_expression
    : logical_and_expression { $$ = $1; }
    | logical_or_expression OR logical_and_expression
    {
        auto binop = std::make_shared<idyl::parser::binary_op>();
        binop->op_ = "|";
        binop->left_ = $1;
        binop->right_ = $3;
        binop->line_ = @2.begin.line;
        binop->column_ = @2.begin.column;
        auto expr = std::make_shared<idyl::parser::binary_op_expr>();
        expr->op_ = binop;
        expr->line_ = @2.begin.line;
        expr->column_ = @2.begin.column;
        $$ = expr;
    }
    ;

logical_and_expression
    : bitwise_or_expression { $$ = $1; }
    | logical_and_expression AND bitwise_or_expression
    {
        auto binop = std::make_shared<idyl::parser::binary_op>();
        binop->op_ = "&";
        binop->left_ = $1;
        binop->right_ = $3;
        binop->line_ = @2.begin.line;
        binop->column_ = @2.begin.column;
        auto expr = std::make_shared<idyl::parser::binary_op_expr>();
        expr->op_ = binop;
        expr->line_ = @2.begin.line;
        expr->column_ = @2.begin.column;
        $$ = expr;
    }
    ;

bitwise_or_expression
    : bitwise_xor_expression { $$ = $1; }
    | bitwise_or_expression OR bitwise_xor_expression
    {
        auto binop = std::make_shared<idyl::parser::binary_op>();
        binop->op_ = "|";
        binop->left_ = $1;
        binop->right_ = $3;
        binop->line_ = @2.begin.line;
        binop->column_ = @2.begin.column;
        auto expr = std::make_shared<idyl::parser::binary_op_expr>();
        expr->op_ = binop;
        expr->line_ = @2.begin.line;
        expr->column_ = @2.begin.column;
        $$ = expr;
    }
    ;

bitwise_xor_expression
    : bitwise_and_expression { $$ = $1; }
    | bitwise_xor_expression XOR bitwise_and_expression
    {
        auto binop = std::make_shared<idyl::parser::binary_op>();
        binop->op_ = "^";
        binop->left_ = $1;
        binop->right_ = $3;
        binop->line_ = @2.begin.line;
        binop->column_ = @2.begin.column;
        auto expr = std::make_shared<idyl::parser::binary_op_expr>();
        expr->op_ = binop;
        expr->line_ = @2.begin.line;
        expr->column_ = @2.begin.column;
        $$ = expr;
    }
    ;

bitwise_and_expression
    : equality_expression { $$ = $1; }
    | bitwise_and_expression AND equality_expression
    {
        auto binop = std::make_shared<idyl::parser::binary_op>();
        binop->op_ = "&";
        binop->left_ = $1;
        binop->right_ = $3;
        binop->line_ = @2.begin.line;
        binop->column_ = @2.begin.column;
        auto expr = std::make_shared<idyl::parser::binary_op_expr>();
        expr->op_ = binop;
        expr->line_ = @2.begin.line;
        expr->column_ = @2.begin.column;
        $$ = expr;
    }
    ;

equality_expression
    : relational_expression { $$ = $1; }
    | equality_expression EQ relational_expression
    {
        auto binop = std::make_shared<idyl::parser::binary_op>();
        binop->op_ = "==";
        binop->left_ = $1;
        binop->right_ = $3;
        binop->line_ = @2.begin.line;
        binop->column_ = @2.begin.column;
        auto expr = std::make_shared<idyl::parser::binary_op_expr>();
        expr->op_ = binop;
        expr->line_ = @2.begin.line;
        expr->column_ = @2.begin.column;
        $$ = expr;
    }
    | equality_expression NEQ relational_expression
    {
        auto binop = std::make_shared<idyl::parser::binary_op>();
        binop->op_ = "!=";
        binop->left_ = $1;
        binop->right_ = $3;
        binop->line_ = @2.begin.line;
        binop->column_ = @2.begin.column;
        auto expr = std::make_shared<idyl::parser::binary_op_expr>();
        expr->op_ = binop;
        expr->line_ = @2.begin.line;
        expr->column_ = @2.begin.column;
        $$ = expr;
    }
    ;

relational_expression
    : shift_expression { $$ = $1; }
    | relational_expression LT shift_expression
    {
        auto binop = std::make_shared<idyl::parser::binary_op>();
        binop->op_ = "<";
        binop->left_ = $1;
        binop->right_ = $3;
        binop->line_ = @2.begin.line;
        binop->column_ = @2.begin.column;
        auto expr = std::make_shared<idyl::parser::binary_op_expr>();
        expr->op_ = binop;
        expr->line_ = @2.begin.line;
        expr->column_ = @2.begin.column;
        $$ = expr;
    }
    | relational_expression GT shift_expression
    {
        auto binop = std::make_shared<idyl::parser::binary_op>();
        binop->op_ = ">";
        binop->left_ = $1;
        binop->right_ = $3;
        binop->line_ = @2.begin.line;
        binop->column_ = @2.begin.column;
        auto expr = std::make_shared<idyl::parser::binary_op_expr>();
        expr->op_ = binop;
        expr->line_ = @2.begin.line;
        expr->column_ = @2.begin.column;
        $$ = expr;
    }
    | relational_expression LE shift_expression
    {
        auto binop = std::make_shared<idyl::parser::binary_op>();
        binop->op_ = "<=";
        binop->left_ = $1;
        binop->right_ = $3;
        binop->line_ = @2.begin.line;
        binop->column_ = @2.begin.column;
        auto expr = std::make_shared<idyl::parser::binary_op_expr>();
        expr->op_ = binop;
        expr->line_ = @2.begin.line;
        expr->column_ = @2.begin.column;
        $$ = expr;
    }
    | relational_expression GE shift_expression
    {
        auto binop = std::make_shared<idyl::parser::binary_op>();
        binop->op_ = ">=";
        binop->left_ = $1;
        binop->right_ = $3;
        binop->line_ = @2.begin.line;
        binop->column_ = @2.begin.column;
        auto expr = std::make_shared<idyl::parser::binary_op_expr>();
        expr->op_ = binop;
        expr->line_ = @2.begin.line;
        expr->column_ = @2.begin.column;
        $$ = expr;
    }
    ;

shift_expression
    : additive_expression { $$ = $1; }
    | shift_expression LSHIFT additive_expression
    {
        auto binop = std::make_shared<idyl::parser::binary_op>();
        binop->op_ = "<<";
        binop->left_ = $1;
        binop->right_ = $3;
        binop->line_ = @2.begin.line;
        binop->column_ = @2.begin.column;
        auto expr = std::make_shared<idyl::parser::binary_op_expr>();
        expr->op_ = binop;
        expr->line_ = @2.begin.line;
        expr->column_ = @2.begin.column;
        $$ = expr;
    }
    | shift_expression RSHIFT additive_expression
    {
        auto binop = std::make_shared<idyl::parser::binary_op>();
        binop->op_ = ">>";
        binop->left_ = $1;
        binop->right_ = $3;
        binop->line_ = @2.begin.line;
        binop->column_ = @2.begin.column;
        auto expr = std::make_shared<idyl::parser::binary_op_expr>();
        expr->op_ = binop;
        expr->line_ = @2.begin.line;
        expr->column_ = @2.begin.column;
        $$ = expr;
    }
    ;

additive_expression
    : multiplicative_expression { $$ = $1; }
    | additive_expression PLUS multiplicative_expression
    {
        auto binop = std::make_shared<idyl::parser::binary_op>();
        binop->op_ = "+";
        binop->left_ = $1;
        binop->right_ = $3;
        binop->line_ = @2.begin.line;
        binop->column_ = @2.begin.column;
        auto expr = std::make_shared<idyl::parser::binary_op_expr>();
        expr->op_ = binop;
        expr->line_ = @2.begin.line;
        expr->column_ = @2.begin.column;
        $$ = expr;
    }
    | additive_expression MINUS multiplicative_expression
    {
        auto binop = std::make_shared<idyl::parser::binary_op>();
        binop->op_ = "-";
        binop->left_ = $1;
        binop->right_ = $3;
        binop->line_ = @2.begin.line;
        binop->column_ = @2.begin.column;
        auto expr = std::make_shared<idyl::parser::binary_op_expr>();
        expr->op_ = binop;
        expr->line_ = @2.begin.line;
        expr->column_ = @2.begin.column;
        $$ = expr;
    }
    ;

multiplicative_expression
    : unary_expression { $$ = $1; }
    | multiplicative_expression MUL unary_expression
    {
        auto binop = std::make_shared<idyl::parser::binary_op>();
        binop->op_ = "*";
        binop->left_ = $1;
        binop->right_ = $3;
        binop->line_ = @2.begin.line;
        binop->column_ = @2.begin.column;
        auto expr = std::make_shared<idyl::parser::binary_op_expr>();
        expr->op_ = binop;
        expr->line_ = @2.begin.line;
        expr->column_ = @2.begin.column;
        $$ = expr;
    }
    | multiplicative_expression DIV unary_expression
    {
        auto binop = std::make_shared<idyl::parser::binary_op>();
        binop->op_ = "/";
        binop->left_ = $1;
        binop->right_ = $3;
        binop->line_ = @2.begin.line;
        binop->column_ = @2.begin.column;
        auto expr = std::make_shared<idyl::parser::binary_op_expr>();
        expr->op_ = binop;
        expr->line_ = @2.begin.line;
        expr->column_ = @2.begin.column;
        $$ = expr;
    }
    | multiplicative_expression MOD unary_expression
    {
        auto binop = std::make_shared<idyl::parser::binary_op>();
        binop->op_ = "%";
        binop->left_ = $1;
        binop->right_ = $3;
        binop->line_ = @2.begin.line;
        binop->column_ = @2.begin.column;
        auto expr = std::make_shared<idyl::parser::binary_op_expr>();
        expr->op_ = binop;
        expr->line_ = @2.begin.line;
        expr->column_ = @2.begin.column;
        $$ = expr;
    }
    ;

unary_expression
    : postfix_expression { $$ = $1; }
    | NOT unary_expression
    {
        auto unop = std::make_shared<idyl::parser::unary_op>();
        unop->op_ = "~";
        unop->operand_ = $2;
        unop->line_ = @1.begin.line;
        unop->column_ = @1.begin.column;
        auto expr = std::make_shared<idyl::parser::unary_op_expr>();
        expr->op_ = unop;
        expr->line_ = @1.begin.line;
        expr->column_ = @1.begin.column;
        $$ = expr;
    }
    | MINUS unary_expression
    {
        auto unop = std::make_shared<idyl::parser::unary_op>();
        unop->op_ = "-";
        unop->operand_ = $2;
        unop->line_ = @1.begin.line;
        unop->column_ = @1.begin.column;
        auto expr = std::make_shared<idyl::parser::unary_op_expr>();
        expr->op_ = unop;
        expr->line_ = @1.begin.line;
        expr->column_ = @1.begin.column;
        $$ = expr;
    }
    | MEMORY_OP LPAREN expression RPAREN
    {
        auto memop = std::make_shared<idyl::parser::memory_op>();
        memop->expr_ = $3;
        memop->line_ = @1.begin.line;
        memop->column_ = @1.begin.column;
        auto expr = std::make_shared<idyl::parser::memory_op_expr>();
        expr->op_ = memop;
        expr->line_ = @1.begin.line;
        expr->column_ = @1.begin.column;
        $$ = expr;
    }
    | MEMORY_OP LPAREN expression COMMA expression RPAREN
    {
        auto memop = std::make_shared<idyl::parser::memory_op>();
        memop->expr_ = $3;
        memop->delay_count_ = $5;
        memop->line_ = @1.begin.line;
        memop->column_ = @1.begin.column;
        auto expr = std::make_shared<idyl::parser::memory_op_expr>();
        expr->op_ = memop;
        expr->line_ = @1.begin.line;
        expr->column_ = @1.begin.column;
        $$ = expr;
    }
    ;

postfix_expression
    : primary_expression { $$ = $1; }
    | postfix_expression LPAREN argument_list RPAREN
    {
        auto call = std::make_shared<idyl::parser::function_call>();
        call->function_ = $1;
        call->arguments_ = $3;
        call->line_ = @2.begin.line;
        call->column_ = @2.begin.column;
        auto expr = std::make_shared<idyl::parser::function_call_expr>();
        expr->call_ = call;
        expr->line_ = @2.begin.line;
        expr->column_ = @2.begin.column;
        $$ = expr;
    }
    | postfix_expression LPAREN RPAREN
    {
        auto call = std::make_shared<idyl::parser::function_call>();
        call->function_ = $1;
        call->arguments_ = {};
        call->line_ = @2.begin.line;
        call->column_ = @2.begin.column;
        auto expr = std::make_shared<idyl::parser::function_call_expr>();
        expr->call_ = call;
        expr->line_ = @2.begin.line;
        expr->column_ = @2.begin.column;
        $$ = expr;
    }
    | postfix_expression DOT IDENTIFIER
    {
        auto access = std::make_shared<idyl::parser::flow_access>();
        access->flow_ = $1;
        access->member_ = $3;
        access->line_ = @2.begin.line;
        access->column_ = @2.begin.column;
        auto expr = std::make_shared<idyl::parser::flow_access_expr>();
        expr->access_ = access;
        expr->line_ = @2.begin.line;
        expr->column_ = @2.begin.column;
        $$ = expr;
    }
    | postfix_expression NAMESPACE_DOT IDENTIFIER
    {
        auto access = std::make_shared<idyl::parser::module_access>();
        access->module_ = $1;
        access->function_ = $3;
        access->arguments_ = {};
        access->line_ = @2.begin.line;
        access->column_ = @2.begin.column;
        auto expr = std::make_shared<idyl::parser::module_access_expr>();
        expr->access_ = access;
        expr->line_ = @2.begin.line;
        expr->column_ = @2.begin.column;
        $$ = expr;
    }
    | postfix_expression NAMESPACE_DOT IDENTIFIER LPAREN argument_list RPAREN
    {
        auto access = std::make_shared<idyl::parser::module_access>();
        access->module_ = $1;
        access->function_ = $3;
        access->arguments_ = $5;
        access->line_ = @2.begin.line;
        access->column_ = @2.begin.column;
        auto expr = std::make_shared<idyl::parser::module_access_expr>();
        expr->access_ = access;
        expr->line_ = @2.begin.line;
        expr->column_ = @2.begin.column;
        $$ = expr;
    }
    | postfix_expression NAMESPACE_DOT IDENTIFIER LPAREN RPAREN
    {
        auto access = std::make_shared<idyl::parser::module_access>();
        access->module_ = $1;
        access->function_ = $3;
        access->arguments_ = {};
        access->line_ = @2.begin.line;
        access->column_ = @2.begin.column;
        auto expr = std::make_shared<idyl::parser::module_access_expr>();
        expr->access_ = access;
        expr->line_ = @2.begin.line;
        expr->column_ = @2.begin.column;
        $$ = expr;
    }
    | postfix_expression LBRACKET expression RBRACKET
    {
        auto access = std::make_shared<idyl::parser::flow_access>();
        access->flow_ = $1;
        access->index_ = $3;
        access->line_ = @2.begin.line;
        access->column_ = @2.begin.column;
        auto expr = std::make_shared<idyl::parser::flow_access_expr>();
        expr->access_ = access;
        expr->line_ = @2.begin.line;
        expr->column_ = @2.begin.column;
        $$ = expr;
    }
    ;

primary_expression
    : IDENTIFIER
    {
        auto id = std::make_shared<idyl::parser::identifier>();
        id->name_ = $1;
        id->line_ = @1.begin.line;
        id->column_ = @1.begin.column;
        auto expr = std::make_shared<idyl::parser::identifier_expr>();
        expr->identifier_ = id;
        expr->line_ = @1.begin.line;
        expr->column_ = @1.begin.column;
        $$ = expr;
    }
    | NUMBER
    {
        auto num = std::make_shared<idyl::parser::number_literal>();
        num->value_ = $1;
        num->line_ = @1.begin.line;
        num->column_ = @1.begin.column;
        auto expr = std::make_shared<idyl::parser::literal_expr>();
        expr->literal_ = num;
        expr->line_ = @1.begin.line;
        expr->column_ = @1.begin.column;
        $$ = expr;
    }
    | TIME_LITERAL
    {
        auto time = std::make_shared<idyl::parser::time_literal>();
        // Parse time literal
        std::string str = $1;
        if (str.find("ms") != std::string::npos) {
            time->value_ = str.substr(0, str.length() - 2);
            time->unit_ = "ms";
        } else if (str.find("hz") != std::string::npos) {
            time->value_ = str.substr(0, str.length() - 2);
            time->unit_ = "hz";
        } else if (str.find("bpm") != std::string::npos) {
            time->value_ = str.substr(0, str.length() - 3);
            time->unit_ = "bpm";
        } else if (str.find("b") != std::string::npos) {
            time->value_ = str.substr(0, str.length() - 1);
            time->unit_ = "b";
        } else if (str.find("s") != std::string::npos) {
            time->value_ = str.substr(0, str.length() - 1);
            time->unit_ = "s";
        }
        time->line_ = @1.begin.line;
        time->column_ = @1.begin.column;
        auto expr = std::make_shared<idyl::parser::literal_expr>();
        expr->literal_ = time;
        expr->line_ = @1.begin.line;
        expr->column_ = @1.begin.column;
        $$ = expr;
    }
    | TRIGGER
    {
        auto trigger = std::make_shared<idyl::parser::trigger_literal>();
        trigger->line_ = @1.begin.line;
        trigger->column_ = @1.begin.column;
        auto expr = std::make_shared<idyl::parser::literal_expr>();
        expr->literal_ = trigger;
        expr->line_ = @1.begin.line;
        expr->column_ = @1.begin.column;
        $$ = expr;
    }
    | REST
    {
        auto rest = std::make_shared<idyl::parser::rest_literal>();
        rest->line_ = @1.begin.line;
        rest->column_ = @1.begin.column;
        auto expr = std::make_shared<idyl::parser::literal_expr>();
        expr->literal_ = rest;
        expr->line_ = @1.begin.line;
        expr->column_ = @1.begin.column;
        $$ = expr;
    }
    | DT 
    {
        auto id = std::make_shared<idyl::parser::identifier>();
        id->name_ = "dt";
        id->line_ = @1.begin.line;
        id->column_ = @1.begin.column;
        auto expr = std::make_shared<idyl::parser::identifier_expr>();
        expr->identifier_ = id;
        expr->line_ = @1.begin.line;
        expr->column_ = @1.begin.column;
        $$ = expr;
    }
    | DUR
    {
        auto id = std::make_shared<idyl::parser::identifier>();
        id->name_ = "dur";
        id->line_ = @1.begin.line;
        id->column_ = @1.begin.column;
        auto expr = std::make_shared<idyl::parser::identifier_expr>();
        expr->identifier_ = id;
        expr->line_ = @1.begin.line;
        expr->column_ = @1.begin.column;
        $$ = expr;
    }
    | AGE
    {
        auto id = std::make_shared<idyl::parser::identifier>();
        id->name_ = "age";
        id->line_ = @1.begin.line;
        id->column_ = @1.begin.column;
        auto expr = std::make_shared<idyl::parser::identifier_expr>();
        expr->identifier_ = id;
        expr->line_ = @1.begin.line;
        expr->column_ = @1.begin.column;
        $$ = expr;
    }
    | STRING_LITERAL
    {
        auto str = std::make_shared<idyl::parser::string_literal>();
        str->value_ = $1;
        str->line_ = @1.begin.line;
        str->column_ = @1.begin.column;
        auto expr = std::make_shared<idyl::parser::literal_expr>();
        expr->literal_ = str;
        expr->line_ = @1.begin.line;
        expr->column_ = @1.begin.column;
        $$ = expr;
    }
    | LPAREN expression RPAREN
    {
        auto paren = std::make_shared<idyl::parser::parenthesized_expr>();
        paren->expr_ = $2;
        paren->line_ = @1.begin.line;
        paren->column_ = @1.begin.column;
        $$ = paren;
    }
    | flow_literal { $$ = $1; }
    | FLOW LBRACE flow_members RBRACE
    {
        auto expr = std::make_shared<idyl::parser::flow_literal_expr>();
        expr->named_members_ = $3;
        expr->line_ = @1.begin.line;
        expr->column_ = @1.begin.column;
        $$ = expr;
    }
    | STOP
    {
        auto stop_expr = std::make_shared<idyl::parser::self_stop_expr>();
        stop_expr->line_ = @1.begin.line;
        stop_expr->column_ = @1.begin.column;
        $$ = stop_expr;
    }
    | LBRACE block_body RBRACE
    {
        auto block = std::make_shared<idyl::parser::block_expr>();
        block->statements_ = $2;
        block->line_ = @1.begin.line;
        block->column_ = @1.begin.column;
        $$ = block;
    }
    ;

argument_list
    : argument_list COMMA argument
    {
        $$ = $1;
        if ($3) $$.push_back($3);
    }
    | argument { $$ = {$1}; }
    ;

argument
    : expression
    {
        auto arg = std::make_shared<idyl::parser::argument>();
        arg->name_ = "";
        arg->value_ = $1;
        arg->line_ = @1.begin.line;
        arg->column_ = @1.begin.column;
        $$ = arg;
    }
    | IDENTIFIER ASSIGN expression
    {
        auto arg = std::make_shared<idyl::parser::argument>();
        arg->name_ = $1;
        arg->value_ = $3;
        arg->line_ = @1.begin.line;
        arg->column_ = @1.begin.column;
        $$ = arg;
    }
    | DT ASSIGN expression 
    {
        auto arg = std::make_shared<idyl::parser::argument>();
        arg->name_ = "dt";
        arg->value_ = $3;
        arg->line_ = @1.begin.line;
        arg->column_ = @1.begin.column;
        $$ = arg;
    }
    ;

%%

void yy::parser::error(const location_type& l, const std::string& m) {
    std::cerr << l << ": " << m << std::endl;
}
