// idyl_parser.y — Bison grammar
%locations
%define api.value.type {idyl::parser::node_ptr}
%define parse.error verbose
%define parse.trace

%code requires {
    #include "ast.hpp"
}

%{
#include <iostream>
#include <string>
#include<cstdio>
#include<cstdlib>
#include "ast.hpp"
using namespace std;
using namespace idyl::parser;

int yylex(void);
void yyerror(const char *s);
node_ptr root;
%}



%token IDENT IDENT_ASSIGN NUMBER TIME TRIGGER MODULE
%token ASSIGN COLON PARALLEL ARROW UPDATE_ARROW WHERE DUP
%token STOP AFTER
%token DOUBLEQUOTE QUOTE
%token PLUS MINUS MUL DIV MOD POW
%token LT GT LE GE EQ NEQ AND OR
%token LPAREN RPAREN LBRACE RBRACE LBRACKET RBRACKET COMMA SEMI
%token EMIT
%token DOT
%token TILDE
%token QUESTION AT
%token SPLIT MERGE
%token DOLLAR

%left COLON              // ← MOVE TO TOP - highest precedence for pipelines
%left PARALLEL           // :: for parallel composition
%left SPLIT MERGE        // routing operators
%left OR                 // logical or
%left AND                // logical and
%left EQ NEQ             // equality
%left LT GT LE GE        // comparison
%left PLUS MINUS         // addition/subtraction
%left MUL DIV MOD        // multiplication/division
%right POW               // exponentiation (right-associative)
%left TILDE UPDATE_ARROW // feedback and state update
%right QUESTION AT       // ternary

%type program statement expr expr_opt funcdef base_expr param stateupdate stateupdate_opt 
%type where_opt lifecycle  operation block block_opt lambdaexpr ident_or_primary 
%type   parallel feedback typeparams_opt updatestmt emitstmt string 
%type ternary_op route binop bracketupdate_opt kvpair stmt module lambda_head ident_assign
%type IDENT NUMBER TIME
%type statements paramlist paramlist_opt arglist arglist_opt stmtlist paramnames kvlist arg

%%

program
    : statements {
        auto pgm = std::make_shared<program>();
        pgm->statements = std::static_pointer_cast<statements>($1)->list;
        root = pgm;
        $$ = pgm;
    }
    ;

statements
    : statement {
        auto ptr = std::make_shared<statements>();
        ptr->list.push_back($1);
        $$ = ptr;
    }
    | statements statement {
        std::static_pointer_cast<statements>($1)->list.push_back( $2 ); 
        $$ = $1;
    }
    ;

statement
    : funcdef {
        auto ptr = std::make_shared<statement>();
        ptr->content = $1;
        $$ = ptr;
    }
    | module {
        auto ptr = std::make_shared<statement>(); 
        ptr->content = $1;
        $$ = ptr;
    }
    | expr {
        auto ptr = std::make_shared<statement>();
        ptr->content = $1;
        $$ = ptr;
    }
    ;

ident_assign 
    : IDENT_ASSIGN expr {
        std::cerr << "DEBUG: ident_assign, expr is actually type: ";
        if (auto p = std::dynamic_pointer_cast<pipeline>($2)) {
            std::cerr << "PIPELINE!" << std::endl;
        } else if (auto e = std::dynamic_pointer_cast<expr>($2)) {
            std::cerr << "EXPR" << std::endl;
        } else {
            std::cerr << "OTHER: " << typeid(*$2).name() << std::endl;
        }
        auto ptr = std::make_shared<assignment>(); 
        ptr->identifier = $1; 
        ptr->expr = $2;
        $$ = ptr;
    }
    ;


funcdef
    : IDENT LPAREN paramlist_opt RPAREN ASSIGN expr stateupdate_opt where_opt  {
        std::cerr << "DEBUG: funcdef rule 1 (with params)" << std::endl;
        auto ptr = std::make_shared<function_def>();
        ptr->name = $1;
        ptr->paramlist_opt = $3; 
        ptr->expr = $6;
        ptr->stateupdate_opt = $7;
        ptr->where_opt = $8;
        $$ = ptr;
    }
    | ident_assign stateupdate_opt where_opt /* zero args function or alias */ 
    {
        std::cerr << "DEBUG: funcdef rule 2 (ident_assign)" << std::endl;
        auto ptr = std::make_shared<function_def>(); 
        auto assign = std::static_pointer_cast<assignment>($1);
        ptr->name = $1;
        ptr->expr = assign->expr; 
        ptr->stateupdate_opt = $2; 
        ptr->where_opt = $3;
        $$ = ptr;
    }
    ;




parallel 
    : expr PARALLEL expr  {
        auto ptr = std::make_shared<parallel>();
        ptr->exprs.push_back($1);
        ptr->exprs.push_back($3);
        $$ = ptr;
    }
    | parallel PARALLEL expr {
        std::static_pointer_cast<parallel>($1)->exprs.push_back($3);
        $$ = $1;
    }
    ;

feedback
    : LPAREN expr RPAREN TILDE {
        auto ptr = std::make_shared<feedback>(); 
        ptr->expr = $2; 
        $$ = ptr;
    }
    ;

lifecycle
    : STOP arglist_opt {
        auto ptr = std::make_shared<lifecycle>();
        ptr->name = $1; 
        ptr->args = $2;
        $$ = ptr;
    }
    | AFTER arglist {
        auto ptr = std::make_shared<lifecycle>();
        ptr->name = $1; 
        ptr->args = $2;
        $$ = ptr;
    }
    ;

where_opt
    : /* empty */ {$$ = std::shared_ptr<where>(nullptr); }
    | WHERE block {
        auto ptr = std::make_shared<where>();
        ptr->block = $2;
        $$ = ptr;
    }
    ;

typeparams 
    : LT IDENT GT {
        auto ptr = std::make_shared<typeparams>();
        ptr->identifier = $2;
        $$ = ptr;
    }
    ;

typeparams_opt
    : /* empty */ { $$ = std::shared_ptr<typeparams>(nullptr);}
    | typeparams {
        $$ = $1;
    }
    ;

paramlist_opt
    : /* empty */ {
        $$ = std::make_shared<paramlist>();
    }
    | paramlist { $$ = $1; }
    ;

paramlist
    : param {
        auto ptr = std::make_shared<paramlist>();
        ptr->list.push_back($1);
        $$ = ptr;
    }
    | paramlist COMMA param {
        std::static_pointer_cast<paramlist>($1)->list.push_back($3);
        $$ = $1;
    }
    ;

param
    : IDENT {
        auto ptr = std::make_shared<param>();
        ptr->expr = $1;
        $$ = ptr;
    }
    | ident_assign {
        auto ptr = std::make_shared<param>(); 
        ptr->expr = $1; 
        $$ = ptr;
    }
    ;

stateupdate
    : UPDATE_ARROW block
    {
        auto ptr = std::make_shared<stateupdate>();
        ptr->block = $2;
        $$ = ptr;
    }
    ;

stateupdate_opt 
    :  {$$ = std::shared_ptr<stateupdate>(nullptr);}
    | stateupdate {$$ = $1;}
    ;

block
    : LBRACE stmtlist RBRACE {
        auto ptr = std::make_shared<block>();
        ptr->stmtlist = $2;
        $$ = ptr;
    }
    ;

block_opt
    : {$$ = std::shared_ptr<block>(nullptr);}
    | LBRACE stmtlist RBRACE {$$ = $2;}
    ;

stmtlist
    : /* empty */ {$$ = std::make_shared<stmtlist>();}
    | stmt {
        auto ptr = std::make_shared<stmtlist>();
        ptr->list.push_back($1);
        $$ = ptr;
    }
    | stmtlist stmt {
        std::static_pointer_cast<stmtlist>($1)->list.push_back($2);
        $$ = $1;
    }
    ;

stmt 
    : updatestmt {$$ = std::make_shared<stmt>($1);}
    | emitstmt {$$ = std::make_shared<stmt>($1);}
    | ident_assign SEMI {$$ = std::make_shared<stmt>($1);}
    | expr SEMI {$$ = std::make_shared<stmt>($1);}
    | lifecycle SEMI {$$ = std::make_shared<stmt>($1);}
    ;

updatestmt
    : ident_assign expr SEMI {
        auto ptr = std::make_shared<updatestmt>();
        ptr->name = $1; 
        ptr->expr = $2;
        $$ = ptr;
    }
    ;

emitstmt
    : EMIT SEMI {
        std::static_pointer_cast<emit>($1)->expr = nullnode();
        $$ = $1;
    }
    | EMIT ASSIGN expr SEMI {
        std::static_pointer_cast<emit>($1)->expr = $3;
        $$ = $1;
    }
    ;

lambda_head
    : LPAREN paramnames RPAREN ARROW {$$ = $2;}
   ;

lambdaexpr
    : lambda_head expr_opt block_opt {
        auto ptr = std::make_shared<lambdaexpr>();
        ptr->paramnames = $1;
        ptr->expr = $2;
        ptr->block_opt = $3;
        $$ = ptr;
    }
    ;

paramnames
    : IDENT {
        auto ptr = std::make_shared<paramnames>();
        ptr->list.push_back($1);
        $$ = ptr;
    }
    | paramnames COMMA IDENT {
        std::static_pointer_cast<paramnames>($1)->list.push_back($3);
        $$ = $1;
    }
    ;


ternary_op 
    : parallel QUESTION expr  {
        auto ptr = std::make_shared<ternary_op>();
        ptr->parallel = $1;
        ptr->condition = $3; 
        $$ = ptr;
    }
    ;
    /*: expr QUESTION expr AT expr*/


expr_opt
    : /*empty*/  {$$ = std::make_shared<expr>();}
    | expr {$$ = $1;}
    ;

expr 
    : base_expr {
        auto ptr = std::make_shared<expr>();
        ptr->base_expr = $1;
        ptr->dup = false;
        $$ = ptr;
    }
    | expr LBRACKET kvlist RBRACKET /* rebinding */
    {
        auto ptr = std::make_shared<expr>();
        ptr->lexpr = $1; 
        ptr->dup = false;
        ptr->kvlist = $3; 
        $$ = ptr;
    }
    | DUP expr LBRACKET kvlist RBRACKET /* duplicate */
    {
        auto ptr = std::make_shared<expr>();
        ptr->dup = true;
        ptr->lexpr = $2; 
        ptr->kvlist = $4; 
        $$ = ptr;
    }
    | expr WHERE block  /* where can exist on arbitrary expressions/calls */
    {
        auto ptr = std::make_shared<expr>();
        ptr->lexpr = $1; 
        ptr->dup = false;
        ptr->block = $3; 
        $$ = ptr;
    } 
    | expr COLON expr { // Pipeline
        std::cerr << "DEBUG: Creating pipeline" << std::endl;
        auto ptr = std::make_shared<pipeline>();
        ptr->expr1 = $1;
        ptr->expr2 = $3;
        ptr->lambdaexpr = nullptr;
        $$ = ptr;
    }
    ;

base_expr
    : ident_or_primary {$$ = $1;}
    | string {$$ = $1;}
    | NUMBER {$$ = $1;}
    | TIME {$$ = $1;}
    | TRIGGER {$$ = $1;}
    | feedback {$$ = $1;}
    | operation { $$ = $1; }
    | LPAREN expr RPAREN { $$ = $2; }
    | ternary_op {$$ = $1;}
    | lambdaexpr {$$ = $1;}
    | parallel {$$ = $1;}
    | route  {$$ = $1;}
    | lifecycle  {$$ = $1;}/* primitive instructions for time like stop and after */
    | module {$$ = $1; }
    ;

/* Primary expressions: single entry for IDENT-starting forms */
ident_or_primary
    : IDENT {
        /* start with an identifier node */
        $$ = $1;
    }
    /* function-call suffix: primary ( typeparams_opt? ) ( arglist_opt ) bracketupdate_opt */
    | ident_or_primary typeparams_opt LPAREN arglist_opt RPAREN bracketupdate_opt {
        /* $1 is the callee (node_ptr), $2 typeparams_opt, $4 arglist_opt, $6 bracketupdate_opt */
        auto call = std::make_shared<function_call>();
        call->callee = $1;
        call->typeparams_opt = $2;
        call->args_opt = $4;
        call->bracketupdate_opt = $6;
        $$ = call;
    }
    /* member access: primary . IDENT  (turns previous primary into module instance or member access) */
    | ident_or_primary DOT IDENT {
        /* build a module_call-like node where $1 is instance and $3 is method identifier */
        auto mc = std::make_shared<module_call>();
        mc->instance_name = $1;
        /* create a placeholder function_call for the method; actual call suffix may follow */
        auto method_call = std::make_shared<function_call>();
        method_call->callee = $3; /* method identifier node */
        method_call->typeparams_opt = std::shared_ptr<typeparams>(nullptr);
        method_call->args_opt = std::make_shared<arglist>();
        method_call->bracketupdate_opt = std::shared_ptr<bracketupdate>(nullptr);
        mc->method_call = method_call;
        $$ = mc;
    }
    /* cast suffix: primary typeparams  -> treat as cast if desired */
    | ident_or_primary typeparams {
        /* If $1 is an identifier and $2 is typeparams, create a cast node.
           If you prefer cast only when starting with IDENT, you can keep original cast rule.
           Here we allow chaining like IDENT<...> then call suffix. */
        auto c = std::make_shared<cast>();
        c->identifier = $1;
        c->typeparams = $2;
        $$ = c;
    }
    ;


operation
    : expr binop expr  {
        auto ptr = std::make_shared<operation>();
        ptr->lexpr = $1;
        ptr->binop = $2;
        ptr->rexpr = $3;
        $$ = ptr;
    }
    ;

route 
    : expr SPLIT expr {
        auto ptr = std::make_shared<route>();
        ptr->lexpr = $1;
        ptr->rexpr = $3; 
        ptr->type = $2;
        $$ = ptr;
    }
    | expr MERGE expr {
        auto ptr = std::make_shared<route>();
        ptr->lexpr = $1;
        ptr->rexpr = $3; 
        ptr->type = $2;
        $$ = ptr;
    }
    ;

module 
    : MODULE LPAREN string RPAREN  {
        auto ptr = std::make_shared<module_instance>();
        ptr->identifier = $1;
        ptr->module_name = $4;
        $$ = ptr;
    }
    ;


arglist_opt
    : /* empty */ {$$ = std::make_shared<arglist>();}
    | arglist { std::cout << "Arglist " << std::endl; $$ = $1; }
    ;

arglist
    : arg {
        auto ptr = std::make_shared<arglist>();
        ptr->list.push_back($1);
        std::cout << "add arg to arglist" << std::endl;
        $$ = ptr;
    }
    | arglist COMMA arg {
        std::static_pointer_cast<arglist>($1)->list.push_back($3);
        std::cout << "push arg to arglist" << std::endl;
        $$ = $1;
    }
    ;

arg 
    : ident_assign {
        std::cout << "arg : assign" << std::endl;
        auto ptr = std::make_shared<arg>(); 
        ptr->expr = $1; 
        $$ = ptr;
    }
    | expr {
        std::cout << "arg: expr" << std::endl; 
        auto ptr = std::make_shared<arg>();
        ptr->expr = $1;
        $$ = ptr;
    }
    ;

binop
    : PLUS {$$ = $1;}
    | MINUS {$$ = $1;} 
    | MUL {$$ = $1;} 
    | DIV {$$ = $1;} 
    | MOD {$$ = $1;} 
    | POW {$$ = $1;} 
    | EQ {$$ = $1;} 
    | NEQ {$$ = $1;} 
    | LT {$$ = $1;} 
    | GT {$$ = $1;} 
    | LE {$$ = $1;} 
    | GE {$$ = $1;} 
    | AND {$$ = $1;} 
    | OR {$$ = $1;}
    ;

string 
    : DOUBLEQUOTE IDENT DOUBLEQUOTE 
    | QUOTE IDENT QUOTE 
    ;

bracketupdate_opt
    : /* empty */ {$$ = std::shared_ptr<bracketupdate>(nullptr);}
    | LBRACKET kvlist RBRACKET {
        auto ptr = std::make_shared<bracketupdate>();
        ptr->kvlist = std::static_pointer_cast<kvlist>($2)->list;
        $$ = ptr;
    }
    ;

kvlist
    : kvpair {
        auto ptr = std::make_shared<kvlist>();
        ptr->list.push_back($1);
        $$ = ptr;
    }
    | kvlist SEMI kvpair {
        std::static_pointer_cast<kvlist>($1)->list.push_back($3);
        $$ = $1;
    }
    ;

kvpair
    : ident_assign {
        auto ptr = std::make_shared<kvpair>();
        auto assign = std::static_pointer_cast<assignment>($1);
        ptr->identifier = assign->identifier; 
        ptr->expr = assign->expr; 
        $$ = ptr;
    }
    ;

%%