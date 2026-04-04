#pragma once 

#include <unordered_map>
#include <vector>
#include <string> 

#include "semantic/symbol.hpp"
#include "core/builtins.hpp"

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

            for(size_t i = 0; i < core::num_builtins; ++i)
            {
                const auto& builtin = core::builtins[i];
                define(builtin.name_, symbol_info{symbol_t::builtin, builtin.name_});
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
    };
} // --- idyl::semantic ---
