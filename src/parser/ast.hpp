#pragma once
#include<memory>
#include <string>
#include <vector>
#include <iostream>

namespace idyl::parser {

enum class node_t {
    program, function_def, function_call,
    rebinding, duplication, pipeline, route, feedback, bracketupdate,
    parallel, emit, literal, identifier, number, time,
    operation, expr, assignment,
    lifecycle,
    where, stateupdate, block,
    param, paramlist, arglist, kvpair, lbdparam, lambdaexpr,
    updatestmt, 
    binop, ternary_op, typeparams, stmt,
    statement, statements, stmtlist, paramnames, kvlist,
    cast, trigger, string, module, module_call,
    route_op,
    arg, named_arg,
};

struct node {
    node_t node_type;
    int line = 0, column = 0; // optional source location
    virtual ~node() = default;
};

using node_ptr = std::shared_ptr<node>;
inline static node_ptr nullnode() {return node_ptr(nullptr);}

struct program : node {
    std::vector<node_ptr> statements;
    program() {node_type = node_t::program;}
};

struct identifier : node {
    std::string name;
    identifier() {node_type = node_t::identifier; }
};

struct cast : node {
    node_ptr identifier; 
    node_ptr typeparams;
    cast() {node_type = node_t::cast; } 
};

struct number : node {
    std::string value;
    number() {node_type = node_t::number; }
};

struct time_prim : node {
    std::string value;
    time_prim() {node_type = node_t::time; }
};

struct string_prim : node {
    node_ptr identifier;
    string_prim() {node_type = node_t::string;}
};

struct trigger_prim : node {
    std::string value; 
    trigger_prim() {node_type = node_t::trigger; }
};

struct module_instance : node {
    node_ptr identifier;
    node_ptr module_name;
    module_instance() { node_type = node_t::module; }
};

struct module_call : node {
    node_ptr instance_name; 
    node_ptr method_call;
    module_call() {node_type = node_t::module_call; std::cout << "module call " << std::endl; }
};

struct route_op : node {
    std::string op;
    route_op() {node_type = node_t::route_op;}
};

struct function_call : node {
    node_ptr callee;
    node_ptr typeparams_opt;
    node_ptr args_opt;
    node_ptr bracketupdate_opt;
    function_call() { node_type = node_t::function_call; }
};

struct function_def : node {
    node_ptr name; 
    node_ptr paramlist_opt;
    node_ptr expr; 
    node_ptr stateupdate_opt; 
    node_ptr where_opt;
    function_def() {node_type = node_t::function_def; }
};

struct expr : node {
    node_ptr base_expr;
    node_ptr lexpr;
    node_ptr kvlist; 
    node_ptr block;
    bool dup;
    expr() {node_type = node_t::expr; }
};

struct operation : node {
    node_ptr lexpr;
    node_ptr rexpr;
    node_ptr binop;
    operation() {node_type = node_t::operation;}
};

struct assignment : node {
    node_ptr identifier;
    node_ptr expr; 
    assignment() { node_type = node_t::assignment;}
};

struct lifecycle : node {
    node_ptr name; // maybe replace with an enum for primitive functions 
    node_ptr args;
    lifecycle() {node_type = node_t::lifecycle; }
};

struct where : node {
    node_ptr block;
    where() {node_type = node_t::where; }
};

struct stateupdate : node {
    node_ptr block;
    stateupdate() {node_type = node_t::stateupdate; }
};

struct block : node {
    node_ptr stmtlist;
    block() {node_type = node_t::block; }
};

struct param : node {
    node_ptr expr; // either identifier or assignment
    param() {node_type = node_t::param;}
};

struct updatestmt : node {
    node_ptr name; 
    node_ptr expr;
    updatestmt() {node_type = node_t::updatestmt;}
};

struct emit : node {
    std::string name;
    node_ptr expr;
    emit() {node_type = node_t::emit;}
};

struct binop : node {
    std::string op; 
    binop(std::string o) { node_type = node_t::binop; op = o;}
};

struct bracketupdate : node {
    std::vector<node_ptr> kvlist;
    bracketupdate() {node_type = node_t::bracketupdate;}
};

struct kvpair : node {
    node_ptr identifier; 
    node_ptr expr;
    kvpair() {node_type = node_t::kvpair;}
};

struct lbdparam : node {
    std::string identifier;  
    lbdparam(std::string id) {node_type = node_t::lbdparam;}
};

struct ternary_op : node {
    node_ptr parallel;
    node_ptr condition;
    ternary_op() {node_type = node_t :: ternary_op; }
};

struct lambdaexpr : node {
    node_ptr paramnames; 
    node_ptr expr; 
    node_ptr block_opt;
    lambdaexpr() {node_type = node_t::lambdaexpr;}
};

struct pipeline : node {
    node_ptr expr1, expr2; 
    node_ptr lambdaexpr;
    pipeline() {node_type = node_t::pipeline;}
};

struct parallel : node {
    std::vector<node_ptr> exprs;
    parallel() {node_type = node_t::parallel;}
};

struct feedback : node {
    node_ptr expr;
    feedback() {node_type = node_t::feedback;}
};

struct typeparams : node {
    node_ptr identifier;
    typeparams() {node_type = node_t::typeparams; }
};

struct route : node {
    node_ptr type;
    node_ptr lexpr, rexpr;
    route() {node_type = node_t::route; }
};

struct stmt : node {
    node_ptr child;
    stmt(node_ptr c) : child(c) {node_type = node_t::stmt; }
};

struct statement : node {
    node_ptr content;
    statement() {node_type = node_t::statement;}
};

struct statements : node {
    std::vector<node_ptr> list;
    statements() {node_type = node_t::statements;}
};

struct paramlist : node {
    std::vector<node_ptr> list;
    paramlist() {node_type = node_t::paramlist;}
};

struct arglist : node {
    std::vector<node_ptr> list;
    arglist() {node_type = node_t::arglist;}
};

struct arg : node {
    node_ptr expr; // either base expr or assignment
    arg() {node_type = node_t::arg; }
};

struct paramnames : node {
    std::vector<node_ptr> list;
    paramnames() {node_type = node_t::paramnames;}
};

struct kvlist : node {
    std::vector<node_ptr> list;
    kvlist() {node_type = node_t::kvlist;}
};
struct stmtlist : node {
    std::vector<node_ptr> list;
    stmtlist() {node_type = node_t::stmtlist;}
};

static void printAST(const node_ptr& n, int indent = 0) {
    if (!n) return;
    std::string pad(indent, ' ');

    switch (n->node_type) {
        case node_t::program: {
            auto pgm = std::static_pointer_cast<program>(n);
            std::cout << pad << "Program\n";
            for (auto& st : pgm->statements)
                printAST(st, indent + 2);
            break;
        }
        case node_t::function_def: {
            auto f = std::static_pointer_cast<function_def>(n);
            std::cout << pad << "FunctionDef\n";
            printAST(f->name, indent + 2);
            printAST(f->paramlist_opt, indent + 2);
            printAST(f->expr, indent + 2);
            printAST(f->stateupdate_opt, indent + 2);
            printAST(f->where_opt, indent + 2);
            break;
        }
        case node_t::function_call: {
            auto c = std::static_pointer_cast<function_call>(n);
            std::cout << pad << "FunctionCall\n";
            printAST(c->callee, indent + 2);
            printAST(c->typeparams_opt, indent + 2);
            printAST(c->args_opt, indent + 2);
            printAST(c->bracketupdate_opt, indent + 2);
            break;
        }
        case node_t::module_call: {
            auto c = std::static_pointer_cast<module_call>(n);
            std::cout << pad << "ModuleCall\n";
            printAST(c->instance_name, indent + 2);
            printAST(c->method_call, indent + 2);
            break;
        }
        case node_t::identifier: {
            auto id = std::static_pointer_cast<identifier>(n);
            std::cout << pad << "Identifier(" << id->name << ")\n";
            break;
        }
        case node_t::cast: {
            auto id = std::static_pointer_cast<cast>(n);
            printAST(id->identifier, indent + 2);
            printAST(id->typeparams, indent + 2);
            break;
        }
        case node_t::number: {
            auto num = std::static_pointer_cast<number>(n);
            std::cout << pad << "Number(" << num->value << ")\n";
            break;
        }
        case node_t::time: {
            auto t = std::static_pointer_cast<time_prim>(n);
            std::cout << pad << "Time(" << t->value << ")\n";
            break;
        }
        case node_t::trigger: {
            auto t = std::static_pointer_cast<trigger_prim>(n);
            std::cout << pad << "Trigger(" << t->value << ")\n";
            break;
        }
        case node_t::string: {
            auto t = std::static_pointer_cast<string_prim>(n);
            std::cout << pad << "String(\n";
            printAST(t->identifier, indent + 2);
            std::cout << pad << ")\n";
            break;
        }
        case node_t::emit: {
            auto e = std::static_pointer_cast<emit>(n);
            std::cout << pad << "Emit(" << e->name << ")\n";
            break;
        }
        case node_t::binop: {
            auto bop = std::static_pointer_cast<binop>(n);
            std::cout << pad << "BinOp(" << bop->op << ")\n";
            break;
        }
        case node_t::operation: {
            auto op = std::static_pointer_cast<operation>(n);
            std::cout << pad << "Operation\n";
            printAST(op->lexpr, indent + 2);
            printAST(op->binop, indent + 2);
            printAST(op->rexpr, indent + 2);
            break;
        }
        case node_t::expr: {
            auto e = std::static_pointer_cast<expr>(n);
            std::string dup = "";
            if(e->dup) dup = " - dup ";
            std::cout << pad << "Expr " << dup << "\n";
            printAST(e->base_expr, indent + 2);
            printAST(e->lexpr, indent + 2);
            printAST(e->kvlist, indent + 2);
            printAST(e->block, indent + 2);
            break;
        }
        case node_t::assignment: {
            auto a = std::static_pointer_cast<assignment>(n);
            std::cout << pad << "Assignment\n";
            printAST(a->identifier, indent + 2);
            printAST(a->expr, indent + 2);
            break;
        }
        case node_t::module: {
            auto a = std::static_pointer_cast<module_instance>(n);
            std::cout << pad << "Module\n";
            printAST(a->identifier, indent + 2);
            printAST(a->module_name, indent + 2);

        }
        case node_t::block: {
            auto b = std::static_pointer_cast<block>(n);
            std::cout << pad << "Block\n";
            printAST(b->stmtlist, indent + 2);
            break;
        }
        case node_t::stmtlist: {
            auto sl = std::static_pointer_cast<stmtlist>(n);
            std::cout << pad << "StmtList\n";
            for (auto& s : sl->list)
                printAST(s, indent + 2);
            break;
        }
        case node_t::statements: {
            auto s = std::static_pointer_cast<statements>(n);
            std::cout << pad << "Statements\n";
            for (auto& child : s->list)
                printAST(child, indent + 2);
            break;
        }
        case node_t::statement: {
            auto st = std::static_pointer_cast<statement>(n);
            std::cout << pad << "Statement\n";
            printAST(st->content, indent + 2);
            break;
        }
        case node_t::paramlist: {
            auto pl = std::static_pointer_cast<paramlist>(n);
            std::cout << pad << "ParamList\n";
            for (auto& p : pl->list)
                printAST(p, indent + 2);
            break;
        }
        case node_t::arglist: {
            auto al = std::static_pointer_cast<arglist>(n);
            std::cout << pad << "ArgList\n";
            for (auto& a : al->list)
                printAST(a, indent + 2);
            break;
        }
        case node_t::paramnames: {
            auto pn = std::static_pointer_cast<paramnames>(n);
            std::cout << pad << "ParamNames\n";
            for (auto& p : pn->list)
                printAST(p, indent + 2);
            break;
        }
        case node_t::kvlist: {
            auto kl = std::static_pointer_cast<kvlist>(n);
            std::cout << pad << "KvList\n";
            for (auto& kv : kl->list)
                printAST(kv, indent + 2);
            break;
        }
        case node_t::kvpair: {
            auto kv = std::static_pointer_cast<kvpair>(n);
            std::cout << pad << "KvPair\n";
            printAST(kv->identifier, indent + 2);
            printAST(kv->expr, indent + 2);
            break;
        }
        case node_t::pipeline: {
            auto p = std::static_pointer_cast<pipeline>(n);
            std::cout << pad << "Pipeline\n";
            printAST(p->expr1, indent + 2);
            printAST(p->expr2, indent + 2);
            printAST(p->lambdaexpr, indent + 2);
            break;
        }
        case node_t::parallel: {
            auto par = std::static_pointer_cast<parallel>(n);
            std::cout << pad << "Parallel\n";
            for (auto& e : par->exprs)
                printAST(e, indent + 2);
            break;
        }
        case node_t::feedback: {
            auto fb = std::static_pointer_cast<feedback>(n);
            std::cout << pad << "Feedback\n";
            printAST(fb->expr, indent + 2);
            break;
        }
        case node_t::route: {
            auto r = std::static_pointer_cast<route>(n);
            std::cout << pad << "Route\n";
            printAST(r->type, indent + 2);
            printAST(r->lexpr, indent + 2);
            printAST(r->rexpr, indent + 2);
            break;
        }
        case node_t::route_op: {
            auto ro = std::static_pointer_cast<route_op>(n);
            std::cout << pad << "RouteOp(" << ro->op << ")\n";
            break;
        }
        case node_t::lambdaexpr: {
            auto l = std::static_pointer_cast<lambdaexpr>(n);
            std::cout << pad << "LambdaExpr\n";
            printAST(l->paramnames, indent + 2);
            printAST(l->expr, indent + 2);
            printAST(l->block_opt, indent + 2);
            break;
        }
        case node_t::lbdparam: {
            auto lp = std::static_pointer_cast<lbdparam>(n);
            std::cout << pad << "LambdaParam(" << lp->identifier << ")\n";
            break;
        }
        case node_t::ternary_op: {
            auto t = std::static_pointer_cast<ternary_op>(n);
            std::cout << pad << "TernaryOp\n";
            printAST(t->parallel, indent + 2);
            printAST(t->condition, indent + 2);
            break;
        }
        case node_t::typeparams: {
            auto tp = std::static_pointer_cast<typeparams>(n);
            std::cout << pad << "TypeParams\n";
            printAST(tp->identifier, indent + 2);
            break;
        }

        case node_t::lifecycle: {
            auto lc = std::static_pointer_cast<lifecycle>(n);
            std::cout << pad << "Lifecycle\n";
            printAST(lc->name, indent + 2);
            printAST(lc->args, indent + 2);
            break;
        }
        case node_t::where: {
            auto w = std::static_pointer_cast<where>(n);
            std::cout << pad << "Where\n";
            printAST(w->block, indent + 2);
            break;
        }
        case node_t::stateupdate: {
            auto su = std::static_pointer_cast<stateupdate>(n);
            std::cout << pad << "StateUpdate\n";
            printAST(su->block, indent + 2);
            break;
        }
        case node_t::param: {
            auto p = std::static_pointer_cast<param>(n);
            std::cout << pad << "Param\n";
            printAST(p->expr, indent + 2);
            break;
        }
        case node_t::arg: {
            auto p = std::static_pointer_cast<arg>(n);
            std::cout << pad << "Argument\n";
            printAST(p->expr, indent + 2);
            break;
        }
        case node_t::updatestmt: {
            auto u = std::static_pointer_cast<updatestmt>(n);
            std::cout << pad << "UpdateStmt\n";
            printAST(u->name, indent + 2);
            printAST(u->expr, indent + 2);
            break;
        }
        case node_t::bracketupdate: {
            auto bu = std::static_pointer_cast<bracketupdate>(n);
            std::cout << pad << "BracketUpdate\n";
            for (auto& kv : bu->kvlist)
                printAST(kv, indent + 2);
            break;
        }
        case node_t::stmt: {
            auto s = std::static_pointer_cast<stmt>(n);
            std::cout << pad << "Stmt\n";
            printAST(s->child, indent + 2);
            break;
        }
        default:
            std::cout << pad << "Unknown node node_type " << static_cast<int>(n->node_type) << "\n";
    }
}

}