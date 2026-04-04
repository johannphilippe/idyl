#include "semantic/analyzer.hpp"
#include "semantic/symbol.hpp"
#include "semantic/scope.hpp"
#include "debug.hpp"
#include <iostream> 

namespace idyl::semantic {

    void analyzer::analyze(const parser::program& program) {
        std::cout << "Analyzing program...\n";
        // First add built-in functions and constants to global scope, then collect top-level declarations, then resolve everything else 
        scope_stack_.define_builtins();
        global_scope_pass(program);
        for(auto & it : program.statements_) {
            resolve(it);
        }
    }

    void analyzer::global_scope_pass(const parser::program& program) {
        // TODO - Implement global scope pass to collect top-level declarations (not implemented yet)
        scope_stack_.push(scope_t::global); 
        scope_stack_.define_builtins(); // Pre-populate global scope with built-in functions and constants 
        // First pass : Collect top-level declarations (functions, flows, imports)

        for(auto & it : program.statements_) {
            switch(it->type_) { 
                case parser::node_t::library_import:
                {
                    // TODO Check if library exists and can be loaded (not implemented yet, just a placeholder)

                    // Check whether the library is imported in a namespace or merged into global scope
                    if(std::static_pointer_cast<parser::library_import>(it)->namespace_.empty()) {
                        std::cout << "Found library import statement (merged into global scope).\n";
                    } else {
                        std::cout << "Found library import statement (namespace: " << std::static_pointer_cast<parser::library_import>(it)->namespace_ << ").\n";
                    }

                    break;
                }
                case parser::node_t::module_import:
                {   
                    //TODO Check if module exists and can be loaded (not implemented yet, just a placeholder)

                    // Check whether the module is imported in a namespace or merged into global scope
                    if(std::static_pointer_cast<parser::module_import>(it)->namespace_.empty()) {
                        std::cout << "Found module import statement (merged into global scope).\n";
                    } else {
                        std::cout << "Found module import statement (namespace: " << std::static_pointer_cast<parser::module_import>(it)->namespace_ << ").\n";
                    }
                    break;
                }
                case parser::node_t::function_definition:
                {
                    if(symbol_info *existing = scope_stack_.lookup_current_scope(std::static_pointer_cast<parser::function_definition>(it)->name_)) {
                        std::cerr << "Warning: Shadowing - Duplicate function definition for '" 
                            << std::static_pointer_cast<parser::function_definition>(it)->name_ << "' at line " << it->line_ << ", column " << it->column_ << ".\n";
                        std::cerr << "         Previous definition found at line " 
                            << existing->line_ << ", column " << existing->column_ << ".\n";
                    }

                    std::cout << "Found function definition.\n";
                    std::shared_ptr<symbol_info> func_info = std::make_shared<symbol_info>();
                    func_info->type_ = symbol_t::function;
                    func_info->name_ = std::static_pointer_cast<parser::function_definition>(it)->name_;
                    func_info->line_ = it->line_;
                    func_info->column_ = it->column_;
                    func_info->arity_ = std::static_pointer_cast<parser::function_definition>(it)->parameters_.size();
                    scope_stack_.define(func_info->name_, *func_info);
                    break;
                }
                case parser::node_t::flow_definition:
                {
                    if(symbol_info *existing = scope_stack_.lookup_current_scope(std::static_pointer_cast<parser::flow_definition>(it)->name_)) {
                        std::cerr << "Warning: Shadowing - Duplicate flow definition for '" 
                            << std::static_pointer_cast<parser::flow_definition>(it)->name_ << "' at line " << it->line_ << ", column " << it->column_ << ".\n";
                        std::cerr << "         Previous definition found at line " 
                            << existing->line_ << ", column " << existing->column_ << ".\n";
                    }

                    std::cout << "Found flow definition.\n";
                    std::shared_ptr<symbol_info> func_info = std::make_shared<symbol_info>();
                    func_info->type_ = symbol_t::flow;
                    func_info->name_ = std::static_pointer_cast<parser::flow_definition>(it)->name_;
                    func_info->line_ = it->line_;
                    func_info->column_ = it->column_;
                    func_info->arity_ = std::static_pointer_cast<parser::flow_definition>(it)->parameters_.size();
                    scope_stack_.define(func_info->name_, *func_info);
                    break;
                }
                case parser::node_t::process_block:
                {
                    std::cout << "Found process block.\n";
                    break;
                }
                default:
                    std::cout << "Found other top-level statement.\n";
            }
        } 
    }


    void analyzer::resolve(const parser::node_ptr& node) {
        switch(node->type_)
        {
            case parser::node_t::function_definition:
            {
                std::cout << "Resolving function definition: " << std::static_pointer_cast<parser::function_definition>(node)->name_ << "\n";
                scope_stack_.push(scope_t::function_body); 
                for(const auto& param : std::static_pointer_cast<parser::function_definition>(node)->parameters_) {
                    scope_stack_.define(param->name_, symbol_info{symbol_t::parameter, param->name_, param->line_, param->column_});
                }
                resolve(std::static_pointer_cast<parser::function_definition>(node)->body_);
                if(std::static_pointer_cast<parser::function_definition>(node)->lambda_block_) {
                    scope_stack_.push(scope_t::lambda_body);
                    if(std::static_pointer_cast<parser::function_definition>(node)->lambda_block_->init_) {
                        scope_stack_.push(scope_t::init_block);
                        for(const auto& stmt : std::static_pointer_cast<parser::function_definition>(node)->lambda_block_->init_->statements_) {
                            resolve(stmt);
                        }
                        scope_stack_.pop();
                    }
                    for(const auto& stmt : std::static_pointer_cast<parser::function_definition>(node)->lambda_block_->update_statements_) {
                        resolve(stmt);
                    }
                    scope_stack_.pop();
                }
                scope_stack_.pop();
                break;
            }

            case parser::node_t::flow_definition:
            {
                std::cout << "Resolving flow definition: " << std::static_pointer_cast<parser::flow_definition>(node)->name_ << "\n";
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
                std::cout << "Resolving process block.\n";
                scope_stack_.push(scope_t::process_block);
                for(const auto& stmt : std::static_pointer_cast<parser::process_block>(node)->body_->statements_) {
                    resolve(stmt);
                }
                scope_stack_.pop();
                break;
            }
            case parser::node_t::flow_member:
            {
                std::cout << "Resolving flow member: " << std::static_pointer_cast<parser::flow_member>(node)->name_ << "\n";
                resolve(std::static_pointer_cast<parser::flow_member>(node)->value_);
                break;
            }

            case parser::node_t::binary_op_expr:
            {
                std::cout << "Resolving binary operation expression.\n";
                resolve(std::static_pointer_cast<parser::binary_op_expr>(node)->op_);
                break;
            }

            case parser::node_t::unary_op_expr:
            {
                std::cout << "Resolving unary operation expression.\n";
                resolve(std::static_pointer_cast<parser::unary_op_expr>(node)->op_);
                break;
            }

            case parser::node_t::literal_expr:
            {
                std::cout << "Resolving literal expression.\n";
                resolve(std::static_pointer_cast<parser::literal_expr>(node)->literal_);
                break;
            }

            case parser::node_t::identifier_expr:
            {
                std::cout << "Resolving identifier expression.\n";
                resolve(std::static_pointer_cast<parser::identifier_expr>(node)->identifier_);
                break;
            }

            case parser::node_t::function_call_expr:
            {
                std::cout << "Resolving function call expression.\n";
                resolve(std::static_pointer_cast<parser::function_call_expr>(node)->call_);
                break;
            }

            case parser::node_t::function_call: 
            {
                std::cout << "Resolving function call.\n";
                resolve(std::static_pointer_cast<parser::function_call>(node)->function_);
                for(const auto& arg : std::static_pointer_cast<parser::function_call>(node)->arguments_) {
                    resolve(arg);
                }
                break;
            }

            case parser::node_t::argument: 
            {
                std::cout << "Resolving function call argument.\n";

                if(std::static_pointer_cast<parser::argument>(node)->name_.empty()) {
                    std::cout << "Argument is positional.\n";
                } else {

                    std::cout << "Argument is named: " << std::static_pointer_cast<parser::argument>(node)->name_ << "\n";
                }
                resolve(std::static_pointer_cast<parser::argument>(node)->value_);
                break;
            }

            case parser::node_t::parenthesized_expr:
            {
                std::cout << "Resolving parenthesized expression.\n";
                resolve(std::static_pointer_cast<parser::parenthesized_expr>(node)->expr_);
                break;
            }

            case parser::node_t::assignment:
            {
                std::cout << "Resolving assignment statement.\n";
                symbol_t symbol_type = (std::static_pointer_cast<parser::assignment>(node)->is_emit_) ? symbol_t::emit_variable : symbol_t::local_variable;

                scope_stack_.define(std::static_pointer_cast<parser::assignment>(node)->name_, 
                    symbol_info{ symbol_type, 
                    std::static_pointer_cast<parser::assignment>(node)->name_, 
                    node->line_, node->column_});

                resolve(std::static_pointer_cast<parser::assignment>(node)->value_);
                break;
            }

            case parser::node_t::expression_stmt:
            {
                std::cout << "Resolving expression statement.\n";
                resolve(std::static_pointer_cast<parser::expression_stmt>(node)->expression_);
                break;
            }
            
            case parser::node_t::library_import:
            {
                std::cout << "Resolving library import statement.\n";
                // TODO - Handle library imports (not implemented yet)
                break;
            }

            case parser::node_t::module_import:
            {
                std::cout << "Resolving module import statement.\n";
                // TODO - Handle module imports (not implemented yet)
                break;
            }

            case parser::node_t::program:
            {
                std::cout << "Resolving program.\n";
                for(const auto& stmt : std::static_pointer_cast<parser::program>(node)->statements_) {
                    resolve(stmt);
                }
                break;
            }

            case parser::node_t::flow_access_expr:
            {
                std::cout << "Resolving flow access expression.\n";
                resolve(std::static_pointer_cast<parser::flow_access_expr>(node)->access_);
                break;
            }
            case parser::node_t::module_access_expr:
            {
                std::cout << "Resolving module access expression.\n";
                resolve(std::static_pointer_cast<parser::module_access_expr>(node)->access_);
                break;
            }
            
            case parser::node_t::catch_block:
            {
                std::cout << "Resolving catch block.\n";
                scope_stack_.push(scope_t::catch_block);
                for(const auto& stmt : std::static_pointer_cast<parser::catch_block>(node)->handler_) {
                    resolve(stmt);
                }
                scope_stack_.pop();
                break;
            }
            case parser::node_t::flow_literal_expr:
            {
                std::cout << "Resolving flow literal expression.\n";
                resolve(std::static_pointer_cast<parser::flow_literal_expr>(node)->flow_);
                break;
            }
            case parser::node_t::flow_literal:
            {
                std::cout << "Resolving flow literal.\n";
                for(const auto& elem : std::static_pointer_cast<parser::flow_literal>(node)->elements_) {
                    resolve(elem);
                }
                break;
            }

            case parser::node_t::generator_expr_node:
            {
                std::cout << "Resolving generator expression.\n";
                resolve(std::static_pointer_cast<parser::generator_expr_node>(node)->generator_);
                break;
            }
            case parser::node_t::generator_expr: 
            {
                std::cout << "Resolving generator expression.\n";
                resolve(std::static_pointer_cast<parser::generator_expr>(node)->range_start_);
                resolve(std::static_pointer_cast<parser::generator_expr>(node)->range_end_);
                resolve(std::static_pointer_cast<parser::generator_expr>(node)->body_);
                break;
            }

            case parser::node_t::ternary_op_expr:
            {
                std::cout << "Resolving ternary operation expression.\n";
                resolve(std::static_pointer_cast<parser::ternary_op_expr>(node)->op_);
                break;
            }

            case parser::node_t::ternary_op:
            {
                std::cout << "Resolving ternary operation.\n";
                resolve(std::static_pointer_cast<parser::ternary_op>(node)->condition_);
                for(const auto& opt : std::static_pointer_cast<parser::ternary_op>(node)->options_) {
                    resolve(opt);
                }
                break;
            }

            case parser::node_t::memory_op_expr:
            {
                std::cout << "Resolving memory operation expression.\n";
                resolve(std::static_pointer_cast<parser::memory_op_expr>(node)->op_);
                break;
            }

            case parser::node_t::identifier:
            {
                std::cout << "Resolving identifier: " << std::static_pointer_cast<parser::identifier>(node)->name_ << "\n";
                // TODO - Handle identifier resolution (not implemented yet)
                if(symbol_info* info = scope_stack_.lookup(std::static_pointer_cast<parser::identifier>(node)->name_)) {
                    std::cout << "Identifier '" << std::static_pointer_cast<parser::identifier>(node)->name_ 
                        << "' resolved to symbol of type " << idyl::resolve_symbol_t(info->type_) 
                        << " defined at line " << info->line_ << ", column " << info->column_ << ".\n";
                } else {
                    std::cerr << "Error: Identifier '" << std::static_pointer_cast<parser::identifier>(node)->name_ 
                        << "' not found in any accessible scope at line " << node->line_ << ", column " << node->column_ << ".\n";
                    errors_.push_back(error{"Identifier '" + std::static_pointer_cast<parser::identifier>(node)->name_ + "' not found in any accessible scope.", node->line_, node->column_});
                }
                break;
            }

            default:
                std::cout << "Unimplemented node type : "  << idyl::resolve_node_t(  node->type_ ) << "\n";
                 break;
        
        }
    }

    void analyzer::print_analysis_results() const {
        std::cout << "\nSemantic Analysis Results:\n";
        if(errors_.empty()) {
            std::cout << "No semantic errors found.\n";
        } else {
            std::cout << errors_.size() << " semantic error(s) found:\n";
            for(const auto& err : errors_) {
                idyl::error(const_cast<idyl::semantic::analyzer::error&>(err));
            }
        }
    }

}