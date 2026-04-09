#include "semantic/analyzer.hpp"
#include "semantic/symbol.hpp"
#include "semantic/scope.hpp"
#include "parser/parse.hpp"
#include "utilities/filesystem.hpp"
#include "debug.hpp"
#include <iostream> 
#include <fstream>
#include <filesystem>

namespace idyl::semantic {

    void analyzer::analyze(const parser::program& program) {
        idyl::debug("Analyzing program...");
        // Collect top-level declarations, then resolve everything else 
        global_scope_pass(program);
        for(auto & it : program.statements_) {
            resolve(it);
        }
    }

    bool analyzer::has_errors() const {
        for (const auto& d : diagnostics_) {
            if (d.level_ == severity::error) return true;
        }
        return false;
    }

    void analyzer::global_scope_pass(const parser::program& program) {
        scope_stack_.push(scope_t::global); 
        scope_stack_.define_builtins(); // Pre-populate global scope with built-in functions and constants 
        // First pass : Collect top-level declarations (functions, flows, imports)

        for(auto & it : program.statements_) {
            switch(it->type_) { 
                case parser::node_t::library_import:
                {
                    auto lib = std::static_pointer_cast<parser::library_import>(it);
                    std::shared_ptr<parser::program> lib_program = load_library(lib->path_, lib->line_, lib->column_);
                    if (lib_program) {
                        libraries_.push_back(lib_program);
                        register_library_symbols(*lib_program, lib->namespace_);

                        if (lib->namespace_.empty()) {
                            idyl::debug("Loaded library '" + lib->path_ + "' (merged into global scope).");
                        } else {
                            idyl::debug("Loaded library '" + lib->path_ + "' (namespace: " + lib->namespace_ + ").");
                        }
                    }
                    break;
                }
                case parser::node_t::module_import:
                {
                    auto mod = std::static_pointer_cast<parser::module_import>(it);
                    const std::string& ns = mod->namespace_;

                    // ── Built-in module (catalog) ─────────────────────────
                    if (scope_stack_.module_registry_ &&
                            scope_stack_.module_registry_->has_builtin(mod->path_)) {
                        auto entries = scope_stack_.module_registry_
                                            ->list_builtin_entries(mod->path_);
                        // Register namespace identifier so bare `ns` resolves
                        if (!ns.empty()) {
                            symbol_info ns_info;
                            ns_info.type_ = symbol_t::module;
                            ns_info.name_ = ns;
                            ns_info.line_ = mod->line_;
                            ns_info.column_ = mod->column_;
                            scope_stack_.define(ns, ns_info);
                        }
                        for (const auto& entry : entries) {
                            std::string sym_name = ns.empty()
                                ? entry.name_ : (ns + "::" + entry.name_);
                            symbol_info info;
                            info.type_          = symbol_t::builtin;
                            info.name_          = sym_name;
                            info.inferred_type_ = inferred_t::function;
                            info.line_          = mod->line_;
                            info.column_        = mod->column_;
                            // Native temporal functions declare arity via params_
                            if (entry.is_native_temporal_ && !entry.params_.empty()) {
                                int req = 0, total = 0;
                                for (const auto& p : entry.params_) {
                                    ++total;
                                    if (!p.has_default_) ++req;
                                }
                                info.arity_          = total;
                                info.required_arity_ = req;
                            } else {
                                info.arity_          = entry.max_arity_;
                                info.required_arity_ = entry.min_arity_;
                            }
                            scope_stack_.define(sym_name, info);
                        }
                        idyl::debug("Registered built-in module '" + mod->path_ + "' ("
                            + std::to_string(entries.size()) + " symbols"
                            + (ns.empty() ? "" : ", namespace: " + ns) + ").");
                        break;
                    }

                    // ── External (.so) module ─────────────────────────────
                    std::string path = mod->path_;
                    if (!idyl::utilities::get_module_path(path)) {
                        diagnostics_.push_back(diagnostic{severity::warning,
                            "Module '" + mod->path_ + "' not found. "
                            "Functions from this module will not be available.",
                            mod->line_, mod->column_});
                        break;
                    }

                    std::vector<std::string> symbols = idyl::utilities::list_dll_symbols(path);
                    if (symbols.empty()) {
                        diagnostics_.push_back(diagnostic{severity::warning,
                            "Module '" + mod->path_ + "' exported no functions.",
                            mod->line_, mod->column_});
                        break;
                    }

                    if (!ns.empty()) {
                        symbol_info ns_info;
                        ns_info.type_ = symbol_t::module;
                        ns_info.name_ = ns;
                        ns_info.line_ = mod->line_;
                        ns_info.column_ = mod->column_;
                        scope_stack_.define(ns, ns_info);
                    }
                    for (const auto& sym : symbols) {
                        std::string sym_name = ns.empty() ? sym : (ns + "::" + sym);
                        symbol_info info;
                        info.type_ = symbol_t::module;
                        info.name_ = sym_name;
                        info.line_ = mod->line_;
                        info.column_ = mod->column_;
                        scope_stack_.define(sym_name, info);
                    }
                    idyl::debug("Loaded external module '" + mod->path_ + "' ("
                        + std::to_string(symbols.size()) + " symbols"
                        + (ns.empty() ? "" : ", namespace: " + ns) + ").");
                    break;
                }
                case parser::node_t::function_definition:
                {
                    if(symbol_info *existing = scope_stack_.lookup_current_scope(std::static_pointer_cast<parser::function_definition>(it)->name_)) {
                        diagnostics_.push_back(diagnostic{severity::warning,
                            "Shadowing: Duplicate function definition for '" + std::static_pointer_cast<parser::function_definition>(it)->name_ 
                            + "'. Previous definition at line " + std::to_string(existing->line_) + ", column " + std::to_string(existing->column_) + ".",
                            it->line_, it->column_});
                    }

                    idyl::debug("Found function definition.");
                    std::shared_ptr<symbol_info> func_info = std::make_shared<symbol_info>();
                    func_info->type_ = symbol_t::function;
                    func_info->name_ = std::static_pointer_cast<parser::function_definition>(it)->name_;
                    func_info->line_ = it->line_;
                    func_info->column_ = it->column_;
                    func_info->arity_ = std::static_pointer_cast<parser::function_definition>(it)->parameters_.size();

                    func_info->param_info_.reserve(func_info->arity_);
                    bool has_dt = false; 
                    int required_arity = 0;
                    bool is_temporal = std::static_pointer_cast<parser::function_definition>(it)->lambda_block_ != nullptr;
                    for(const auto& param : std::static_pointer_cast<parser::function_definition>(it)->parameters_) {
                        if(param->name_ == "dt") {
                            has_dt = true; 
                            is_temporal = true;
                        }
                        if(!param->default_value_) {
                            required_arity++;
                        }

                        func_info->param_info_.push_back(param_info{
                            param->name_,
                            param->default_value_ != nullptr,
                            param->is_trigger_parameter_,
                            param->is_trigger_parameter_ ? inferred_t::trigger
                                : (param->has_default_time_ || param->name_ == "dt") ? inferred_t::time
                                : param->default_value_ ? infer_expr_type(param->default_value_)
                                : inferred_t::unknown
                        });
                    }
                    func_info->has_dt_param_ = has_dt;
                    func_info->is_temporal_ = is_temporal;
                    func_info->required_arity_ = required_arity;
                    func_info->inferred_type_ = inferred_t::function;
                    scope_stack_.define(func_info->name_, *func_info);
                    break;
                }
                case parser::node_t::flow_definition:
                {
                    if(symbol_info *existing = scope_stack_.lookup_current_scope(std::static_pointer_cast<parser::flow_definition>(it)->name_)) {
                        diagnostics_.push_back(diagnostic{severity::warning,
                            "Shadowing: Duplicate flow definition for '" + std::static_pointer_cast<parser::flow_definition>(it)->name_ 
                            + "'. Previous definition at line " + std::to_string(existing->line_) + ", column " + std::to_string(existing->column_) + ".",
                            it->line_, it->column_});
                    }

                    idyl::debug("Found flow definition.");
                    std::shared_ptr<symbol_info> func_info = std::make_shared<symbol_info>();
                    func_info->type_ = symbol_t::flow;
                    func_info->name_ = std::static_pointer_cast<parser::flow_definition>(it)->name_;
                    func_info->line_ = it->line_;
                    func_info->column_ = it->column_;
                    func_info->arity_ = std::static_pointer_cast<parser::flow_definition>(it)->parameters_.size();

                    func_info->param_info_.reserve(func_info->arity_);
                    bool has_dt = false; 
                    int required_arity = 0;
                    bool is_temporal = std::static_pointer_cast<parser::function_definition>(it)->lambda_block_ != nullptr;
                    for(const auto& param : std::static_pointer_cast<parser::flow_definition>(it)->parameters_) {
                        if(param->name_ == "dt") {
                            has_dt = true; 
                            is_temporal = true;
                        }
                        if(!param->default_value_) {
                            required_arity++;
                        }

                        func_info->param_info_.push_back(param_info{
                            param->name_,
                            param->default_value_ != nullptr,
                            param->is_trigger_parameter_,
                            param->is_trigger_parameter_ ? inferred_t::trigger
                                : (param->has_default_time_ || param->name_ == "dt") ? inferred_t::time
                                : param->default_value_ ? infer_expr_type(param->default_value_)
                                : inferred_t::unknown
                        });
                    }
                    func_info->required_arity_ = required_arity;

                    // Collect flow member names for flow_access validation
                    auto flow_def = std::static_pointer_cast<parser::flow_definition>(it);
                    for (const auto& member : flow_def->members_) {
                        if (!member->name_.empty()) {
                            func_info->member_names_.push_back(member->name_);
                        }
                    }

                    scope_stack_.define(func_info->name_, *func_info);
                    break;
                }
                case parser::node_t::process_block:
                {
                    idyl::debug("Found process block.");
                    break;
                }
                default:
                    idyl::debug("Found other top-level statement.");
            }
        } 
    }


    void analyzer::resolve(const parser::node_ptr& node) {
        switch(node->type_)
        {
            case parser::node_t::function_definition:
            {
                auto func = std::static_pointer_cast<parser::function_definition>(node);
                idyl::debug("Resolving function definition: " + func->name_);
                scope_stack_.push(scope_t::function_body); 

                // Determine temporal status
                bool has_dt = false;
                bool has_trigger = false;
                for (const auto& param : func->parameters_) {
                    if (param->name_ == "dt" || param->has_default_time_) has_dt = true;
                    if (param->is_trigger_parameter_) has_trigger = true;
                }
                bool has_update = func->lambda_block_ && !func->lambda_block_->update_statements_.empty();
                bool is_temporal = has_dt || has_trigger || has_update;

                // If lambda block has update statements but no explicit dt, dt defaults to dt_default (10ms)
                if (has_update && !has_dt && !has_trigger) {
                    idyl::debug("Function '" + func->name_ + "' has update statements but no dt parameter — using implicit dt_default (10ms).");
                    // Make 'dt' available as an implicit parameter in this scope
                    scope_stack_.define("dt", symbol_info{symbol_t::parameter, "dt", func->line_, func->column_});
                }

                // Mark this scope as temporal so nested checks can query it
                scope_stack_.scopes_.back().is_temporal_function_ = is_temporal;
                scope_stack_.scopes_.back().enclosing_function_ = func->name_;

                // Warn about trigger params on non-temporal functions (no lambda block at all)
                if (has_trigger && !func->lambda_block_) {
                    idyl::warning("Trigger parameter in non-temporal function '" + func->name_ + "' — triggers are ignored in pure functions.", func->line_, func->column_);
                    diagnostics_.push_back(diagnostic{severity::warning,
                        "Trigger parameter in non-temporal function '" + func->name_ + "' — triggers are ignored in pure functions.",
                        func->line_, func->column_});
                }

                // Define parameters
                for(const auto& param : func->parameters_) {
                    scope_stack_.define(param->name_, symbol_info{symbol_t::parameter, param->name_, param->line_, param->column_});
                }

                // If there's a lambda block, push its scopes and collect definitions
                // BEFORE resolving the body expression, so init-defined symbols are visible.
                if(func->lambda_block_) {
                    scope_stack_.push(scope_t::lambda_body);
                    scope_stack_.scopes_.back().is_temporal_function_ = is_temporal;
                    scope_stack_.scopes_.back().enclosing_function_ = func->name_;

                    // Pre-collect update-section assignments so the return
                    // variable (func->body_) can resolve against them.  The
                    // return variable is often assigned in the update section
                    // rather than the init block (e.g.  `= out |> { init: { phase = 0 }  out = sin(phase) }`).
                    for(const auto& stmt : func->lambda_block_->update_statements_) {
                        if(stmt->type_ == parser::node_t::function_definition) {
                            auto inner_func = std::static_pointer_cast<parser::function_definition>(stmt);
                            if (inner_func->parameters_.empty() && inner_func->body_) {
                                // Looks like a bare assignment parsed as 0-param def
                                symbol_t sym_type = symbol_t::local_variable;
                                scope_stack_.define(inner_func->name_, symbol_info{sym_type, inner_func->name_, inner_func->line_, inner_func->column_});
                            }
                        } else if(stmt->type_ == parser::node_t::assignment) {
                            auto assign = std::static_pointer_cast<parser::assignment>(stmt);
                            symbol_t sym_type = assign->is_emit_ ? symbol_t::emit_variable : symbol_t::local_variable;
                            scope_stack_.define(assign->name_, symbol_info{sym_type, assign->name_, assign->line_, assign->column_});
                        }
                    }

                    if(func->lambda_block_->init_) {
                        scope_stack_.push(scope_t::init_block);
                        scope_stack_.scopes_.back().is_temporal_function_ = is_temporal;
                        scope_stack_.scopes_.back().enclosing_function_ = func->name_;
                        // First pass: collect function/variable definitions from init block
                        for(const auto& stmt : func->lambda_block_->init_->statements_) {
                            if(stmt->type_ == parser::node_t::function_definition) {
                                auto inner_func = std::static_pointer_cast<parser::function_definition>(stmt);
                                symbol_info inner_info{
                                    symbol_t::function, inner_func->name_, 
                                    inner_func->line_, inner_func->column_,
                                    static_cast<int>(inner_func->parameters_.size())};
                                // Populate param_info for check_call validation
                                int req = 0;
                                for (const auto& p : inner_func->parameters_) {
                                    inner_info.param_info_.push_back(param_info{
                                        p->name_,
                                        p->default_value_ != nullptr,
                                        p->is_trigger_parameter_,
                                        p->is_trigger_parameter_ ? inferred_t::trigger
                                            : (p->has_default_time_ || p->name_ == "dt") ? inferred_t::time
                                            : p->default_value_ ? infer_expr_type(p->default_value_)
                                            : inferred_t::unknown
                                    });
                                    if (!p->default_value_) req++;
                                }
                                inner_info.required_arity_ = req;
                                inner_info.is_temporal_ = inner_func->lambda_block_ != nullptr;
                                inner_info.inferred_type_ = inferred_t::function;
                                scope_stack_.define(inner_func->name_, inner_info);
                            } else if(stmt->type_ == parser::node_t::assignment) {
                                auto assign = std::static_pointer_cast<parser::assignment>(stmt);
                                symbol_t sym_type = assign->is_emit_ ? symbol_t::emit_variable : symbol_t::local_variable;
                                scope_stack_.define(assign->name_, symbol_info{sym_type, assign->name_, assign->line_, assign->column_});
                            }
                        }
                    }
                }

                // Resolve body expression (init/lambda scopes are already active)
                resolve(func->body_);

                // Now fully resolve init and lambda statements
                if(func->lambda_block_) {
                    if(func->lambda_block_->init_) {
                        // Init scope is already pushed — resolve the statements
                        for(const auto& stmt : func->lambda_block_->init_->statements_) {
                            // Validate: no process blocks or imports in init
                            if (stmt->type_ == parser::node_t::process_block || 
                                stmt->type_ == parser::node_t::library_import || 
                                stmt->type_ == parser::node_t::module_import) {
                                diagnostics_.push_back(diagnostic{severity::error,"Invalid statement in init block — only assignments, expressions, function definitions, and catch blocks are allowed.",
                                    stmt->line_, stmt->column_});
                            }
                            resolve(stmt);
                        }
                        scope_stack_.pop(); // pop init_block
                    }
                    for(const auto& stmt : func->lambda_block_->update_statements_) {
                        // Validate: no process blocks or imports in update section
                        if (stmt->type_ == parser::node_t::process_block || 
                            stmt->type_ == parser::node_t::library_import || 
                            stmt->type_ == parser::node_t::module_import) {
                            diagnostics_.push_back(diagnostic{severity::error,"Invalid statement in lambda update section — only assignments, expressions, function definitions, and catch blocks are allowed.",
                                stmt->line_, stmt->column_});
                        }
                        resolve(stmt);
                    }
                    scope_stack_.pop(); // pop lambda_body
                }

                // Check for unused parameters (info diagnostic, spec §11.2.6)
                // Skip 'dt' — it's an implicit scheduling parameter that may
                // not appear in the function body but is used by the runtime.
                {
                    auto& func_scope = scope_stack_.scopes_.back(); // function_body is still on top
                    for (const auto& [name, sym] : func_scope.symbols_) {
                        if (sym.type_ == symbol_t::parameter && !sym.referenced_
                            && name != "dt") {
                            diagnostics_.push_back(diagnostic{severity::info,
                                "Parameter '" + name + "' is never used in function '" + func->name_ + "'.",
                                sym.line_, sym.column_});
                        }
                    }
                }

                scope_stack_.pop(); // pop function_body
                break;
            }

            case parser::node_t::flow_definition:
            {
                idyl::debug("Resolving flow definition: " + std::static_pointer_cast<parser::flow_definition>(node)->name_);
                scope_stack_.push(scope_t::function_body);
                for(const auto& param : std::static_pointer_cast<parser::flow_definition>(node)->parameters_) {
                    scope_stack_.define(param->name_, symbol_info{symbol_t::parameter, param->name_, param->line_, param->column_});
                }
                for(const auto& member : std::static_pointer_cast<parser::flow_definition>(node)->members_) {
                    resolve(member);
                }
                scope_stack_.pop();
                break;
            }
            case parser::node_t::process_block:
            {
                idyl::debug("Resolving process block.");
                // Process blocks must be at global scope
                if(scope_stack_.scopes_.back().type_ != scope_t::global) {
                    diagnostics_.push_back(diagnostic{severity::error,"Process blocks must be at global scope.",
                        node->line_, node->column_});
                }
                scope_stack_.push(scope_t::process_block);
                scope_stack_.scopes_.back().is_process_block_ = true;
                for(const auto& stmt : std::static_pointer_cast<parser::process_block>(node)->body_->statements_) {
                    resolve(stmt);
                }
                scope_stack_.pop();
                break;
            }
            case parser::node_t::flow_member:
            {
                auto member = std::static_pointer_cast<parser::flow_member>(node);
                idyl::debug("Resolving flow member: " + member->name_);
                if (scope_stack_.lookup_current_scope(member->name_)) {
                    diagnostics_.push_back(diagnostic{severity::error,
                        "Duplicate flow member name '" + member->name_ + "'.",
                        node->line_, node->column_});
                } else {
                    scope_stack_.define(member->name_, symbol_info{
                        symbol_t::flow_member, member->name_, node->line_, node->column_
                    });
                }
                resolve(member->value_);
                break;
            }

            case parser::node_t::binary_op_expr:
            {
                idyl::debug("Resolving binary operation expression.");
                resolve(std::static_pointer_cast<parser::binary_op_expr>(node)->op_);
                break;
            }

            case parser::node_t::binary_op:
            {
                idyl::debug("Resolving binary operation.");
                auto op = std::static_pointer_cast<parser::binary_op>(node);
                if (op->left_) resolve(op->left_);
                if (op->right_) resolve(op->right_);
                break;
            }

            case parser::node_t::unary_op_expr:
            {
                idyl::debug("Resolving unary operation expression.");
                resolve(std::static_pointer_cast<parser::unary_op_expr>(node)->op_);
                break;
            }

            case parser::node_t::unary_op:
            {
                idyl::debug("Resolving unary operation.");
                auto op = std::static_pointer_cast<parser::unary_op>(node);
                if (op->operand_) resolve(op->operand_);
                break;
            }

            case parser::node_t::literal_expr:
            {
                idyl::debug("Resolving literal expression.");
                resolve(std::static_pointer_cast<parser::literal_expr>(node)->literal_);
                break;
            }

            case parser::node_t::identifier_expr:
            {
                idyl::debug("Resolving identifier expression.");
                resolve(std::static_pointer_cast<parser::identifier_expr>(node)->identifier_);
                break;
            }

            case parser::node_t::function_call_expr:
            {
                idyl::debug("Resolving function call expression.");
                resolve(std::static_pointer_cast<parser::function_call_expr>(node)->call_);
                break;
            }

            case parser::node_t::function_call: 
            {
                idyl::debug("Resolving function call.");
                auto call = std::static_pointer_cast<parser::function_call>(node);

                // Resolve the callee expression first
                resolve(call->function_);

                // Resolve each argument's value
                for(const auto& arg : call->arguments_) {
                    resolve(arg);
                }

                // Try to extract the callee name for check_call
                std::string callee_name;
                if (call->function_ && call->function_->type_ == parser::node_t::identifier_expr) {
                    auto id_expr = std::static_pointer_cast<parser::identifier_expr>(call->function_);
                    if (id_expr->identifier_) {
                        callee_name = id_expr->identifier_->name_;
                    }
                }
                if (!callee_name.empty()) {
                    if (symbol_info* info = scope_stack_.lookup(callee_name)) {
                        check_call(node, *info);
                    }
                }
                break;
            }

            case parser::node_t::argument: 
            {
                idyl::debug("Resolving function call argument.");

                if(std::static_pointer_cast<parser::argument>(node)->name_.empty()) {
                    idyl::debug("Argument is positional.");
                } else {
                    idyl::debug("Argument is named: " + std::static_pointer_cast<parser::argument>(node)->name_);
                }
                resolve(std::static_pointer_cast<parser::argument>(node)->value_);
                break;
            }

            case parser::node_t::parenthesized_expr:
            {
                idyl::debug("Resolving parenthesized expression.");
                resolve(std::static_pointer_cast<parser::parenthesized_expr>(node)->expr_);
                break;
            }

            case parser::node_t::assignment:
            {
                auto assign = std::static_pointer_cast<parser::assignment>(node);
                idyl::debug("Resolving assignment statement.");

                // Validate emit is only used inside a lambda block
                if (assign->is_emit_ && !scope_stack_.is_in_lambda_context()) {
                    diagnostics_.push_back(diagnostic{severity::error,"'emit' used outside lambda block — emit is only valid inside |> { ... }.",
                        node->line_, node->column_});
                }

                symbol_t symbol_type = assign->is_emit_ ? symbol_t::emit_variable : symbol_t::local_variable;
                scope_stack_.define(assign->name_, 
                    symbol_info{ symbol_type, assign->name_, node->line_, node->column_});

                resolve(assign->value_);
                break;
            }

            case parser::node_t::expression_stmt:
            {
                idyl::debug("Resolving expression statement.");
                resolve(std::static_pointer_cast<parser::expression_stmt>(node)->expression_);
                break;
            }
            
            case parser::node_t::library_import:
            {
                idyl::debug("Resolving library import statement.");
                if(scope_stack_.scopes_.back().type_ != scope_t::global) {
                    diagnostics_.push_back(diagnostic{severity::error,"Library imports must be at global scope.",
                        node->line_, node->column_});
                }
                break;
            }

            case parser::node_t::module_import:
            {
                idyl::debug("Resolving module import statement.");
                if(scope_stack_.scopes_.back().type_ != scope_t::global) {
                    diagnostics_.push_back(diagnostic{severity::error,"Module imports must be at global scope.",
                        node->line_, node->column_});
                }
                break;
            }

            case parser::node_t::program:
            {
                idyl::debug("Resolving program.");
                for(const auto& stmt : std::static_pointer_cast<parser::program>(node)->statements_) {
                    resolve(stmt);
                }
                break;
            }

            case parser::node_t::flow_access_expr:
            {
                idyl::debug("Resolving flow access expression.");
                resolve(std::static_pointer_cast<parser::flow_access_expr>(node)->access_);
                break;
            }

            case parser::node_t::flow_access:
            {
                auto access = std::static_pointer_cast<parser::flow_access>(node);
                idyl::debug("Resolving flow access.");

                // Resolve the flow expression (LHS)
                if (access->flow_) resolve(access->flow_);

                // Resolve the index expression if present
                if (access->index_) resolve(access->index_);

                // Validate the member name against the flow's declared members
                if (!access->member_.empty() && access->flow_ && access->flow_->type_ == parser::node_t::identifier_expr) {
                    auto id_expr = std::static_pointer_cast<parser::identifier_expr>(access->flow_);
                    if (id_expr->identifier_) {
                        std::string flow_name = id_expr->identifier_->name_;
                        if (symbol_info* info = scope_stack_.lookup(flow_name)) {
                            if (info->type_ == symbol_t::flow && !info->member_names_.empty()) {
                                bool found = false;
                                for (const auto& m : info->member_names_) {
                                    if (m == access->member_) { found = true; break; }
                                }
                                if (!found) {
                                    std::string members_list;
                                    for (size_t i = 0; i < info->member_names_.size(); ++i) {
                                        if (i > 0) members_list += ", ";
                                        members_list += info->member_names_[i];
                                    }
                                    diagnostics_.push_back(diagnostic{severity::error,
                                        "'" + access->member_ + "' is not a member of flow '" + flow_name 
                                        + "' (members: " + members_list + ").",
                                        node->line_, node->column_});
                                }
                            }
                        }
                    }
                }
                break;
            }

            case parser::node_t::module_access_expr:
            {
                idyl::debug("Resolving module access expression.");
                auto access = std::static_pointer_cast<parser::module_access_expr>(node)->access_;
                resolve(access);
                break;
            }

            case parser::node_t::module_access:
            {
                auto access = std::static_pointer_cast<parser::module_access>(node);

                // Extract the namespace name from the LHS (must be a simple identifier)
                std::string ns_name;
                if (access->module_ && access->module_->type_ == parser::node_t::identifier_expr) {
                    auto id_expr = std::static_pointer_cast<parser::identifier_expr>(access->module_);
                    if (id_expr->identifier_) {
                        ns_name = id_expr->identifier_->name_;
                    }
                }

                if (ns_name.empty()) {
                    // Non-identifier on LHS of :: — resolve normally
                    resolve(access->module_);
                } else {
                    // First try as a flat qualified name (namespace::member)
                    std::string qualified = ns_name + "::" + access->function_;
                    if (symbol_info* info = scope_stack_.lookup(qualified)) {
                        idyl::debug("Qualified name '" + qualified
                                  + "' resolved to symbol of type " + idyl::resolve_symbol_t(info->type_)
                                  + " defined at line " + std::to_string(info->line_) + ", column " + std::to_string(info->column_) + ".");

                        // For library functions (not module bindings), run arity/arg validation
                        if (info->type_ == symbol_t::function || info->type_ == symbol_t::flow) {
                            // Build a temporary function_call node to reuse check_call
                            auto synth_call = std::make_shared<parser::function_call>();
                            synth_call->line_ = node->line_;
                            synth_call->column_ = node->column_;
                            synth_call->arguments_ = access->arguments_;
                            check_call(synth_call, *info);
                        }
                    } else if (symbol_info* lhs_info = scope_stack_.lookup(ns_name)) {
                        // LHS is a known variable — this may be an emit accessor
                        // (e.g. a::incr where a is bound to a temporal instance).
                        // We can't statically verify the emitted field name, so
                        // just mark the LHS as referenced and allow it.
                        lhs_info->referenced_ = true;
                        idyl::debug("Emit accessor '" + ns_name + "::" + access->function_
                                  + "' — deferring to runtime.");
                    } else {
                        diagnostics_.push_back(diagnostic{severity::error,"'" + qualified + "' not found in any accessible scope.",
                                                node->line_, node->column_});
                    }
                }

                // Resolve arguments regardless
                for (const auto& arg : access->arguments_) {
                    resolve(arg);
                }
                break;
            }
            
            case parser::node_t::catch_block:
            {
                idyl::debug("Resolving catch block.");
                auto cb = std::static_pointer_cast<parser::catch_block>(node);
                // Resolve the source expression (the watched variable)
                if (cb->expression_) {
                    resolve(cb->expression_);
                }
                scope_stack_.push(scope_t::catch_block);
                for(const auto& stmt : cb->handler_) {
                    resolve(stmt);
                }
                scope_stack_.pop();
                break;
            }

            case parser::node_t::at_block:
            {
                idyl::debug("Resolving @ block.");
                auto atb = std::static_pointer_cast<parser::at_block>(node);
                // Resolve the time expr expression (the watched variable)
                if (atb->time_expr_) {
                    resolve(atb->time_expr_);
                }
                scope_stack_.push(scope_t::at_block);
                for(const auto& stmt : atb->handler_) {
                    resolve(stmt);
                }
                scope_stack_.pop();
                break;
            }

            case parser::node_t::flow_literal_expr:
            {
                idyl::debug("Resolving flow literal expression.");
                resolve(std::static_pointer_cast<parser::flow_literal_expr>(node)->flow_);
                break;
            }
            case parser::node_t::flow_literal:
            {
                idyl::debug("Resolving flow literal.");
                for(const auto& elem : std::static_pointer_cast<parser::flow_literal>(node)->elements_) {
                    resolve(elem);
                }
                break;
            }

            case parser::node_t::generator_expr_node:
            {
                idyl::debug("Resolving generator expression node.");
                resolve(std::static_pointer_cast<parser::generator_expr_node>(node)->generator_);
                break;
            }
            case parser::node_t::generator_expr: 
            {
                idyl::debug("Resolving generator expression.");
                auto gen = std::static_pointer_cast<parser::generator_expr>(node);
                // Resolve range bounds first (outside generator scope)
                resolve(gen->range_start_);
                resolve(gen->range_end_);
                // Push a scope for the generator variable, then resolve the body
                scope_stack_.push(scope_t::function_body);
                scope_stack_.define(gen->variable_, symbol_info{symbol_t::local_variable, gen->variable_, node->line_, node->column_});
                resolve(gen->body_);
                scope_stack_.pop();
                break;
            }

            case parser::node_t::ternary_op_expr:
            {
                idyl::debug("Resolving ternary operation expression.");
                resolve(std::static_pointer_cast<parser::ternary_op_expr>(node)->op_);
                break;
            }

            case parser::node_t::ternary_op:
            {
                idyl::debug("Resolving ternary operation.");
                resolve(std::static_pointer_cast<parser::ternary_op>(node)->condition_);
                for(const auto& opt : std::static_pointer_cast<parser::ternary_op>(node)->options_) {
                    resolve(opt);
                }
                break;
            }

            case parser::node_t::memory_op_expr:
            {
                idyl::debug("Resolving memory operation expression.");
                auto mem_expr = std::static_pointer_cast<parser::memory_op_expr>(node);

                // Validate @ is only used inside a temporal function
                if (!scope_stack_.is_in_temporal_context() && !scope_stack_.is_process_context()) {
                    idyl::warning("Memory operator '@' used outside temporal function — no previous value exists.", node->line_, node->column_);
                    diagnostics_.push_back(diagnostic{severity::warning,
                        "Memory operator '@' used outside temporal function — no previous value exists.",
                        node->line_, node->column_});
                }

                resolve(mem_expr->op_);
                break;
            }

            case parser::node_t::memory_op:
            {
                idyl::debug("Resolving memory operation.");
                auto mem = std::static_pointer_cast<parser::memory_op>(node);
                if (mem->expr_) resolve(mem->expr_);
                if (mem->delay_count_) resolve(mem->delay_count_);
                break;
            }

            case parser::node_t::identifier:
            {
                auto ident = std::static_pointer_cast<parser::identifier>(node);
                idyl::debug("Resolving identifier: " + ident->name_);

                // Special check: 'dt' used outside a temporal context
                if (ident->name_ == "dt" && !scope_stack_.is_in_temporal_context()) {
                    idyl::warning("'dt' used outside temporal function — value is undefined.", node->line_, node->column_);
                    diagnostics_.push_back(diagnostic{severity::warning,
                        "'dt' used outside temporal function — value is undefined.",
                        node->line_, node->column_});
                }

                if(symbol_info* info = scope_stack_.lookup(ident->name_)) {
                    info->referenced_ = true;
                    idyl::debug("Identifier '" + ident->name_ 
                        + "' resolved to symbol of type " + idyl::resolve_symbol_t(info->type_) 
                        + " defined at line " + std::to_string(info->line_) + ", column " + std::to_string(info->column_) + ".");
                } else {
                    diagnostics_.push_back(diagnostic{severity::error,"Identifier '" + ident->name_ + "' not found in any accessible scope.", node->line_, node->column_});
                }
                break;
            }

            default:
                idyl::debug("Unimplemented node type: " + idyl::resolve_node_t(node->type_));
                 break;
        
        }
    }

    void analyzer::print_analysis_results() const {
        std::cout << "\nSemantic Analysis Results:\n";
        int num_errors = 0, num_warnings = 0, num_info = 0;
        for (const auto& d : diagnostics_) {
            if (d.level_ == severity::error) num_errors++;
            else if (d.level_ == severity::warning) num_warnings++;
            else num_info++;
        }
        if(diagnostics_.empty()) {
            std::cout << "No semantic issues found.\n";
        } else {
            for(const auto& d : diagnostics_) {
                idyl::print_diagnostic(d);
            }
            std::cout << num_errors << " error(s), " << num_warnings << " warning(s), " << num_info << " info(s).\n";
        }
        std::cout.flush();
    }

    // -----------------------------------------------------------------------
    // Library loading
    // -----------------------------------------------------------------------

    std::shared_ptr<parser::program> analyzer::load_library(const std::string& name, int import_line, int import_col) {
        namespace fs = std::filesystem;

        std::string path = name; 
        if(!idyl::utilities::get_library_path(path)) 
        {
            diagnostics_.push_back(diagnostic{severity::error,
                "Library file not found for '" + name + "'.",
                import_line, import_col});
            return nullptr;
        }

        // Resolve the library path relative to the main source file's ditoryrec
        fs::path lib_path(path);
        if (lib_path.is_relative() && !source_path_.empty()) {
            fs::path source_dir = fs::path(source_path_).parent_path();
            lib_path = source_dir / lib_path;
        }

        // Canonicalize so we can detect duplicate imports reliably
        std::error_code ec;
        fs::path canonical = fs::canonical(lib_path, ec);
        if (ec) {
            diagnostics_.push_back(diagnostic{severity::error,
                "Library file not found: '" + lib_path.string() + "'",
                import_line, import_col});
            return nullptr;
        }

        std::string key = canonical.string();

        // Check for circular imports: if the path is currently being processed
        for (const auto& active : import_stack_) {
            if (active == key) {
                // Build the circular chain for the error message
                std::string chain;
                for (const auto& f : import_stack_) {
                    chain += fs::path(f).filename().string() + " → ";
                }
                chain += fs::path(key).filename().string();
                diagnostics_.push_back(diagnostic{severity::error,
                    "Circular import detected: " + chain,
                    import_line, import_col});
                return nullptr;
            }
        }

        // Prevent duplicate imports
        if (loaded_libraries_.count(key)) {
            idyl::debug("Library '" + path + "' already loaded, skipping.");
            // Find and return the already-parsed AST
            for (auto& lib : libraries_) {
                return lib;
            }
            return nullptr;
        }
        loaded_libraries_.insert(key);

        // Push onto import stack before parsing (for circular detection in transitive imports)
        import_stack_.push_back(key);

        // Parse the library file
        auto lib_program = parser::parse_file(canonical.string());
        if (!lib_program) {
            import_stack_.pop_back();
            diagnostics_.push_back(diagnostic{severity::error,
                "Failed to parse library file: '" + path + "'",
                import_line, import_col});
            return nullptr;
        }
        #ifdef IDYL_VERBOSE
        lib_program->print();
        #endif

        import_stack_.pop_back();

        return lib_program;
    }

    void analyzer::register_library_symbols(const parser::program& lib_program, const std::string& ns) {
        // If a namespace was given (e.g. `myLib = lib("...")`) register the namespace
        // identifier itself so that the bare name resolves during analysis.
        if (!ns.empty()) {
            symbol_info ns_info;
            ns_info.type_ = symbol_t::library;
            ns_info.name_ = ns;
            scope_stack_.define(ns, ns_info);
        }

        for (const auto& stmt : lib_program.statements_) {
            switch (stmt->type_) {
                case parser::node_t::function_definition: {
                    auto func = std::static_pointer_cast<parser::function_definition>(stmt);
                    std::string sym_name = ns.empty() ? func->name_ : (ns + "::" + func->name_);

                    symbol_info info;
                    info.type_ = symbol_t::function;
                    info.name_ = sym_name;
                    info.line_ = func->line_;
                    info.column_ = func->column_;
                    info.arity_ = static_cast<int>(func->parameters_.size());
                    int req = 0;
                    for (const auto& p : func->parameters_) {
                        info.param_info_.push_back(param_info{
                            p->name_, p->default_value_ != nullptr, p->is_trigger_parameter_,
                            p->is_trigger_parameter_ ? inferred_t::trigger
                                : (p->has_default_time_ || p->name_ == "dt") ? inferred_t::time
                                : p->default_value_ ? infer_expr_type(p->default_value_)
                                : inferred_t::unknown
                        });
                        if (!p->default_value_) req++;
                    }
                    info.required_arity_ = req;
                    info.inferred_type_ = inferred_t::function;
                    scope_stack_.define(sym_name, info);
                    break;
                }
                case parser::node_t::flow_definition: {
                    auto flow = std::static_pointer_cast<parser::flow_definition>(stmt);
                    std::string sym_name = ns.empty() ? flow->name_ : (ns + "::" + flow->name_);

                    symbol_info info;
                    info.type_ = symbol_t::flow;
                    info.name_ = sym_name;
                    info.line_ = flow->line_;
                    info.column_ = flow->column_;
                    info.arity_ = static_cast<int>(flow->parameters_.size());
                    int req = 0;
                    for (const auto& p : flow->parameters_) {
                        info.param_info_.push_back(param_info{
                            p->name_, p->default_value_ != nullptr, p->is_trigger_parameter_,
                            p->is_trigger_parameter_ ? inferred_t::trigger
                                : (p->has_default_time_ || p->name_ == "dt") ? inferred_t::time
                                : p->default_value_ ? infer_expr_type(p->default_value_)
                                : inferred_t::unknown
                        });
                        if (!p->default_value_) req++;
                    }
                    info.required_arity_ = req;

                    // Collect flow member names
                    for (const auto& member : flow->members_) {
                        if (!member->name_.empty()) {
                            info.member_names_.push_back(member->name_);
                        }
                    }

                    scope_stack_.define(sym_name, info);
                    break;
                }
                default:
                    // Other top-level statements in the library are ignored for now
                    break;
            }
        }
    }

    void analyzer::check_call(const parser::node_ptr& call_node, const symbol_info& callee_info)
    {
        // Only check function, flow, builtin, and local_variable calls.
        // local_variable is allowed because functions are first-class values:
        // a variable may hold a function reference obtained from a ternary,
        // assignment, or parameter.  We cannot statically verify arity for
        // indirect calls, so we skip detailed checks for them.
        if (callee_info.type_ != symbol_t::function && 
            callee_info.type_ != symbol_t::flow &&
            callee_info.type_ != symbol_t::builtin &&
            callee_info.type_ != symbol_t::local_variable &&
            callee_info.type_ != symbol_t::parameter) {
            diagnostics_.push_back(diagnostic{severity::error,"Attempting to call non-callable symbol '" + callee_info.name_ + "'.", call_node->line_, call_node->column_});
            return;
        }

        // Builtins and indirect calls (local variables / parameters) don't
        // have param_info_ populated — skip detailed arity checks.
        if (callee_info.type_ == symbol_t::builtin ||
            callee_info.type_ == symbol_t::local_variable ||
            callee_info.type_ == symbol_t::parameter) return;

        auto call = std::static_pointer_cast<parser::function_call>(call_node);
        const auto& args = call->arguments_;
        const auto& params = callee_info.param_info_;
        const int num_args = static_cast<int>(args.size());
        const int num_params = callee_info.arity_;
        const int required = callee_info.required_arity_;

        // --- 1. Too many arguments ---
        if (num_args > num_params) {
            diagnostics_.push_back(diagnostic{severity::error,"Too many arguments in call to '" + callee_info.name_ 
                + "'. Expected at most " + std::to_string(num_params) + ", got " + std::to_string(num_args) + ".",
                call_node->line_, call_node->column_});
            return;
        }

        // --- 2. Walk arguments: positional then named ---
        // Track which parameter slots have been filled and by which argument
        std::vector<bool> filled(num_params, false);
        std::vector<int> arg_for_param(num_params, -1); // maps param index -> arg index
        int positional_index = 0; // next param slot for positional args
        bool seen_named = false;

        for (int i = 0; i < num_args; ++i) {
            const auto& arg = args[i];

            if (arg->name_.empty()) {
                // Positional argument
                if (seen_named) {
                    diagnostics_.push_back(diagnostic{severity::error,"Positional argument after named argument in call to '" + callee_info.name_ + "'.",
                        arg->line_, arg->column_});
                    return;
                }

                if (positional_index >= num_params) {
                    // Already caught by "too many" check above, but be safe
                    break;
                }
                filled[positional_index] = true;
                arg_for_param[positional_index] = i;
                positional_index++;
            } else {
                // Named argument
                seen_named = true;

                // Find which parameter this name matches
                int match = -1;
                for (int p = 0; p < num_params; ++p) {
                    if (params[p].name_ == arg->name_) {
                        match = p;
                        break;
                    }
                }

                if (match < 0) {
                    diagnostics_.push_back(diagnostic{severity::error,"Unknown parameter name '" + arg->name_ + "' in call to '" + callee_info.name_ + "'.",
                        arg->line_, arg->column_});
                    continue;
                }

                if (filled[match]) {
                    diagnostics_.push_back(diagnostic{severity::error,"Parameter '" + arg->name_ + "' already provided in call to '" + callee_info.name_ + "'.",
                        arg->line_, arg->column_});
                    continue;
                }

                filled[match] = true;
                arg_for_param[match] = i;
            }
        }

        // --- 3. Check all required parameters are filled ---
        for (int p = 0; p < num_params; ++p) {
            if (!filled[p] && !params[p].has_default_) {
                diagnostics_.push_back(diagnostic{severity::error,"Missing required argument '" + params[p].name_ + "' in call to '" + callee_info.name_ + "'.",
                    call_node->line_, call_node->column_});
            }
        }

        // --- 4. Type inference checks ---
        for (int p = 0; p < num_params; ++p) {
            if (!filled[p] || arg_for_param[p] < 0) continue;
            if (params[p].expected_type_ == inferred_t::unknown) continue;

            const auto& arg = args[arg_for_param[p]];
            inferred_t arg_type = infer_expr_type(arg->value_);
            if (arg_type == inferred_t::unknown) continue; // can't infer — skip

            if (arg_type != params[p].expected_type_) {
                diagnostics_.push_back(diagnostic{severity::error,
                    "Type mismatch for parameter '" + params[p].name_ + "' in call to '" + callee_info.name_
                    + "'. Expected " + idyl::resolve_inferred_t(params[p].expected_type_)
                    + ", got " + idyl::resolve_inferred_t(arg_type) + ".",
                    arg->line_, arg->column_});
            }
        }
    }

    // -----------------------------------------------------------------------
    // Type inference for expressions
    // -----------------------------------------------------------------------

    inferred_t analyzer::infer_expr_type(const parser::node_ptr& node)
    {
        if (!node) return inferred_t::unknown;

        switch (node->type_) {
            case parser::node_t::literal_expr: {
                auto lit = std::static_pointer_cast<parser::literal_expr>(node);
                if (lit->literal_) return infer_expr_type(lit->literal_);
                return inferred_t::unknown;
            }
            case parser::node_t::number_literal:
                return inferred_t::number;
            case parser::node_t::time_literal:
                return inferred_t::time;
            case parser::node_t::trigger_literal:
                return inferred_t::trigger;
            case parser::node_t::rest_literal:
                return inferred_t::trigger;
            case parser::node_t::string_literal:
                return inferred_t::string;
            case parser::node_t::flow_literal_expr:
            case parser::node_t::flow_literal:
                return inferred_t::flow;
            case parser::node_t::generator_expr_node:
            case parser::node_t::generator_expr:
                return inferred_t::flow;
            case parser::node_t::identifier_expr: {
                auto id_expr = std::static_pointer_cast<parser::identifier_expr>(node);
                if (id_expr->identifier_) {
                    if (symbol_info* info = scope_stack_.lookup(id_expr->identifier_->name_)) {
                        return info->inferred_type_;
                    }
                }
                return inferred_t::unknown;
            }
            case parser::node_t::parenthesized_expr:
                return infer_expr_type(std::static_pointer_cast<parser::parenthesized_expr>(node)->expr_);

            // --- Binary operator type propagation (spec §8.3.3) ---
            case parser::node_t::binary_op_expr: {
                auto boe = std::static_pointer_cast<parser::binary_op_expr>(node);
                if (boe->op_) return infer_expr_type(boe->op_);
                return inferred_t::unknown;
            }
            case parser::node_t::binary_op: {
                auto op = std::static_pointer_cast<parser::binary_op>(node);
                inferred_t left = infer_expr_type(op->left_);
                inferred_t right = infer_expr_type(op->right_);

                // Comparison operators always return number (0 or 1)
                if (op->op_ == "<" || op->op_ == ">" || op->op_ == "<=" || 
                    op->op_ == ">=" || op->op_ == "==" || op->op_ == "!=") {
                    return inferred_t::number;
                }

                // Bitwise operators: number & number → number
                if (op->op_ == "&" || op->op_ == "|" || op->op_ == "^" || 
                    op->op_ == "<<" || op->op_ == ">>") {
                    return inferred_t::number;
                }

                // If either is unknown, propagate unknown
                if (left == inferred_t::unknown || right == inferred_t::unknown)
                    return inferred_t::unknown;

                // Same types: result is usually that type
                if (left == right) {
                    // trigger op trigger → trigger
                    if (left == inferred_t::trigger) return inferred_t::trigger;
                    // number op number → number
                    if (left == inferred_t::number) return inferred_t::number;
                    // time op time → depends on operator
                    if (left == inferred_t::time) {
                        if (op->op_ == "/" ) return inferred_t::number; // ratio
                        return inferred_t::time;
                    }
                    return left;
                }

                // Mixed: time * number or number * time → time
                if ((left == inferred_t::time && right == inferred_t::number) ||
                    (left == inferred_t::number && right == inferred_t::time)) {
                    if (op->op_ == "*") return inferred_t::time;
                    if (op->op_ == "/") return (left == inferred_t::time) ? inferred_t::time : inferred_t::unknown;
                    return inferred_t::unknown;
                }

                return inferred_t::unknown;
            }

            // --- Unary operator type propagation (spec §8.3.4) ---
            case parser::node_t::unary_op_expr: {
                auto uoe = std::static_pointer_cast<parser::unary_op_expr>(node);
                if (uoe->op_) return infer_expr_type(uoe->op_);
                return inferred_t::unknown;
            }
            case parser::node_t::unary_op: {
                auto op = std::static_pointer_cast<parser::unary_op>(node);
                inferred_t operand = infer_expr_type(op->operand_);
                if (op->op_ == "-") {
                    if (operand == inferred_t::number) return inferred_t::number;
                    if (operand == inferred_t::time) return inferred_t::time;
                }
                if (op->op_ == "~") return inferred_t::number;
                return operand;
            }

            // --- Ternary operator type propagation (spec §8.3.5) ---
            case parser::node_t::ternary_op_expr: {
                auto toe = std::static_pointer_cast<parser::ternary_op_expr>(node);
                if (toe->op_) return infer_expr_type(toe->op_);
                return inferred_t::unknown;
            }
            case parser::node_t::ternary_op: {
                auto op = std::static_pointer_cast<parser::ternary_op>(node);
                // Result type is the type of the options if they all agree
                if (op->options_.empty()) return inferred_t::unknown;
                inferred_t first = infer_expr_type(op->options_[0]);
                for (size_t i = 1; i < op->options_.size(); ++i) {
                    inferred_t t = infer_expr_type(op->options_[i]);
                    if (t != first) return inferred_t::unknown; // mixed types
                }
                return first;
            }

            // --- Memory operator: @(expr) returns same type as expr (spec §8.3.7) ---
            case parser::node_t::memory_op_expr: {
                auto moe = std::static_pointer_cast<parser::memory_op_expr>(node);
                if (moe->op_) return infer_expr_type(moe->op_);
                return inferred_t::unknown;
            }
            case parser::node_t::memory_op: {
                auto mem = std::static_pointer_cast<parser::memory_op>(node);
                return infer_expr_type(mem->expr_);
            }
            case parser::node_t::stop_statement:
            {
                return inferred_t::trigger;
            }
            case parser::node_t::start_statement:
            {
                if(auto start = std::static_pointer_cast<parser::start_statement>(node); start->target_.empty()) {
                    // Error, start statement must have a target 
                        diagnostics_.push_back(diagnostic{severity::error,
                            "Start statement missing target flow name.",
                            node->line_, node->column_});
                }
                return inferred_t::trigger;
            }

            // --- Function call: return type unknown unless we track return types ---
            case parser::node_t::function_call_expr:
            case parser::node_t::function_call:
                return inferred_t::unknown;

            // --- Flow access: returns element type (unknown in general) ---
            case parser::node_t::flow_access_expr:
            case parser::node_t::flow_access:
                return inferred_t::unknown;

            // --- Module access: unknown (runtime) ---
            case parser::node_t::module_access_expr:
            case parser::node_t::module_access:
                return inferred_t::unknown;

            default:
                return inferred_t::unknown;
        }
    }
}