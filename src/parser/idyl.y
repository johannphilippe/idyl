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



%token IDENT NUMBER TIME TRIGGER STRING MODULE
%token ASSIGN COLON ARROW UPDATE_ARROW WHERE DUP
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

%left PLUS MINUS
%left MUL DIV MOD
%right POW
%left EQ NEQ LT GT LE GE
%left AND OR
%left COLON
%left TILDE UPDATE_ARROW
%right QUESTION AT
%left SPLIT MERGE

%type program statement expr expr_opt funcdef base_expr param stateupdate stateupdate_opt 
%type where_opt lifecycle  operation block block_opt lambdaexpr lbdparam 
%type  pipeline parallel feedback typeparams_opt updatestmt emitstmt string ident_assign
%type ternary_op route funcall binop bracketupdate_opt kvpair stmt module module_call
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

funcdef
    : IDENT LPAREN paramlist_opt RPAREN ASSIGN expr stateupdate_opt where_opt  {
        auto ptr = std::make_shared<function_def>();
        ptr->name = $1;
        ptr->paramlist_opt = $3; 
        ptr->expr = $6;
        ptr->stateupdate_opt = $7;
        ptr->where_opt = $8;
        $$ = ptr;
    }
    | IDENT ASSIGN expr stateupdate_opt where_opt /* zero args function or alias */ 
    {
        auto ptr = std::make_shared<function_def>(); 
        ptr->name = $1;
        ptr->expr = $3; 
        ptr->stateupdate_opt = $4; 
        ptr->where_opt = $5;
        $$ = ptr;
    }
    ;

ident_assign 
    : IDENT ASSIGN expr  {
        auto ptr = std::make_shared<assignment>();
        ptr->identifier = $1;
        ptr->expr = $3; 
        $$ = ptr;
    }
    ;

// Feedback loop must follow a connection expression
pipeline
    : expr COLON lambdaexpr {
        auto ptr = std::make_shared<pipeline>();
        ptr->expr1 = $1;
        ptr->lambdaexpr = $3;
        ptr->expr2 = nullptr;
        $$ = ptr;
    }
    | expr COLON expr {
        auto ptr = std::make_shared<pipeline>();
        ptr->expr1 = $1;
        ptr->expr2 = $3;
        ptr->lambdaexpr = nullptr;
    }
    ;

parallel 
    : expr COMMA expr  {
        auto ptr = std::make_shared<parallel>();
        ptr->exprs.push_back($1);
        ptr->exprs.push_back($3);
        $$ = ptr;
    }
    | parallel COMMA expr {
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
    | LT IDENT GT {
        auto ptr = std::make_shared<typeparams>();
        ptr->identifier = $2;
        $$ = ptr;
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
        auto assign = std::static_pointer_cast<assignment>($1);
        ptr->expr = assign;
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
    | funcall SEMI {$$ = std::make_shared<stmt>($1);}
    | lifecycle SEMI {$$ = std::make_shared<stmt>($1);}
    | pipeline SEMI {$$ = std::make_shared<stmt>($1);}
    ;

updatestmt
    : ident_assign SEMI {
        auto ptr = std::make_shared<updatestmt>();
        auto assign = std::static_pointer_cast<assignment>($1);
        ptr->name = assign->identifier; 
        ptr->expr = assign->expr;
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

lambdaexpr
    : LPAREN paramnames RPAREN ARROW expr_opt block_opt {
        auto ptr = std::make_shared<lambdaexpr>();
        ptr->paramnames = $2;
        ptr->expr = $5;
        ptr->block_opt = $6;
        $$ = ptr;
    }
    ;

paramnames
    : lbdparam {
        auto ptr = std::make_shared<paramnames>();
        ptr->list.push_back($1);
        $$ = ptr;
    }
    | paramnames COMMA lbdparam {
        std::static_pointer_cast<paramnames>($1)->list.push_back($3);
        $$ = $1;
    }
    ;

lbdparam
    : IDENT {$$ = $1;}
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
    | DUP expr LBRACKET kvlist RBRACKET
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
    ;

base_expr
    : IDENT {$$ = $1;}
    | string {$$ = $1;}
    | cast {$$ = $1;}
    | NUMBER {$$ = $1;}
    | TIME {$$ = $1;}
    | TRIGGER {$$ = $1;}
    | funcall {$$ = $1;}
    | module_call {$$ = $1;}
    | feedback {$$ = $1;}
    | operation { $$ = $1; }
    | LPAREN expr RPAREN { $$ = $2; }
    | ternary_op {$$ = $1;}
    | lambdaexpr {$$ = $1;}
    | pipeline {$$ = $1;}
    | parallel {$$ = $1;}
    | route  {$$ = $1;}
    | lifecycle  {$$ = $1;}/* primitive instructions for time like stop and after */
    ;

cast 
    : IDENT typeparams {
        auto ptr = std::make_shared<cast>(); 
        ptr->identifier = $1; 
        ptr->typeparams = $2;
        $$ = ptr;
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

funcall
    : IDENT typeparams_opt LPAREN arglist_opt RPAREN bracketupdate_opt  {
        auto ptr = std::make_shared<function_call>();
        ptr->callee = $1;
        ptr->typeparams_opt = $2;
        ptr->args_opt = $4; 
        ptr->bracketupdate_opt = $6;
        std::cout << "funcall " << std::static_pointer_cast<identifier>($1)->name << std::endl;
        $$ = ptr; 
    }
    ;

module 
    : IDENT ASSIGN MODULE LPAREN string RPAREN  {
        auto ptr = std::make_shared<module_instance>();
        ptr->identifier = $1;
        ptr->module_name = $5;
        $$ = ptr;
    }
    ;

module_call 
    : IDENT DOT funcall {
        auto ptr = std::make_shared<module_call>();
        ptr->instance_name = $1; 
        ptr->method_call = $3; 
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
    | arg COMMA arg {
        std::static_pointer_cast<arglist>($1)->list.push_back($3);
        std::cout << "push arg to arglist" << std::endl;
        $$ = $1;
    }
    ;

arg 
    : base_expr {
        std::cout << "arg: expr" << std::endl; 
        auto ptr = std::make_shared<arg>();
        ptr->expr = $1;
        $$ = ptr;
    }
    | ident_assign { 
        std::cout << "arg assignment" << std::endl; 
        auto ptr = std::make_shared<arg>(); 
        auto assign = std::static_pointer_cast<assignment>($1);
        ptr->expr = assign;
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