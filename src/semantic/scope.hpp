#pragma once 

#include <unordered_map>
#include <vector>
#include <string> 

#include "semantic/symbol.hpp"
#include "core/builtins.hpp"
#include "include/module.hpp"

namespace idyl::semantic {
    enum class scope_t {
        global,
        function_body, 
        lambda_body, 
        init_block, 
        process_block,
        catch_block,
    };

    struct scope {
        scope_t type_;
        std::unordered_map<std::string, symbol_info> symbols_; 

        std::string enclosing_function_; 
        bool is_temporal_function_ = false; 
    };

    struct scope_stack {
        std::vector<scope> scopes_; 

        // Module registry pointer (set before analysis if modules are loaded)
        module::registry* module_registry_ = nullptr;

        void push(scope_t type) {
            scopes_.emplace_back(scope{type});
        }

        void pop() 
        {
            if(scopes_.size() > 1) { // Don't pop the global scope
                scopes_.pop_back();
            }
        }

        void define(const std::string &name, symbol_info info) 
        {
            if (!scopes_.empty()) {
                scopes_.back().symbols_[name] = std::move(info);
            }
        }

        void define_builtins() 
        {
            // Register core builtins  
            for(size_t i = 0; i < core::num_builtins; ++i)
            {
                const auto& builtin = core::builtins[i];
                symbol_info si{symbol_t::builtin, builtin.name_};
                si.inferred_type_ = inferred_t::function;
                define(builtin.name_, std::move(si));
            }

            // Register evaluator intrinsics (clock/tempo) that are handled
            // specially in eval_call but still need to pass semantic analysis.
            for (const auto& name : {"clock", "tempo", "bpm"}) {
                symbol_info si{symbol_t::builtin, name};
                si.inferred_type_ = inferred_t::function;
                define(name, std::move(si));
            }

            // Predefined constants (mirrors environment::init)
            for (const auto& name : {"pi", "tau"}) {
                define(name, symbol_info{symbol_t::local_variable, name});
            }

            // Register module functions so the analyzer recognizes them
            if (module_registry_) {
                for (const auto& [name, entry] : module_registry_->all()) {
                    symbol_info si;
                    si.type_ = symbol_t::builtin;
                    si.name_ = name;
                    si.arity_ = entry.max_arity_;
                    si.required_arity_ = entry.min_arity_;
                    si.inferred_type_ = inferred_t::function;
                    define(name, std::move(si));
                }
            }
        }

        // Global lookup: search from innermost to outermost scope
        symbol_info* lookup(const std::string& name) 
        {
            for (auto it = scopes_.rbegin(); it != scopes_.rend(); ++it) {
                auto sym_it = it->symbols_.find(name);
                if (sym_it != it->symbols_.end()) {
                    return &sym_it->second;
                }
            }
            return nullptr; // Not found
        }

        symbol_info* lookup_current_scope(const std::string& name) 
        {
            if (!scopes_.empty()) {
                auto& symbols = scopes_.back().symbols_;
                auto sym_it = symbols.find(name);
                if (sym_it != symbols.end()) {
                    return &sym_it->second;
                }
            }
            return nullptr; // Not found
        }

        // Check if we are currently inside a temporal function (any enclosing scope is temporal)
        bool is_in_temporal_context() const {
            for (auto it = scopes_.rbegin(); it != scopes_.rend(); ++it) {
                if (it->is_temporal_function_) return true;
            }
            return false;
        }

        // Check if we are currently inside a lambda body or init block
        bool is_in_lambda_context() const {
            for (auto it = scopes_.rbegin(); it != scopes_.rend(); ++it) {
                if (it->type_ == scope_t::lambda_body || it->type_ == scope_t::init_block) return true;
                // Stop at function boundary — don't leak into enclosing functions
                if (it->type_ == scope_t::function_body) return false;
            }
            return false;
        }
    };
} // --- idyl::semantic ---
