#pragma once

#include <unordered_map>
#include <vector>
#include <string>
#include <iostream>

#include "core/core.hpp"
#include "core/builtins.hpp"
#include "include/module.hpp"

namespace idyl::core {

    // ── Runtime scope frame ────────────────────────────────────────────────────
    // Each scope holds variable bindings. The scope chain is walked outward
    // to resolve names (like Scheme's lexical scoping).
    struct scope_frame {
        std::unordered_map<std::string, value> bindings_;
    };

    // ── Runtime environment ────────────────────────────────────────────────────
    // Manages the scope chain and built-in function lookup for the evaluator.
    struct environment {

        // Scope chain: back() is innermost (current) scope
        std::vector<scope_frame> scopes_;

        // Built-in function lookup (name → index into core::builtins[])
        std::unordered_map<std::string, size_t> builtin_index_;

        // Module registry (owned externally — populated before init)
        module::registry* module_registry_ = nullptr;

        // ── Initialization ─────────────────────────────────────────────────────
        void init() {
            // Push global scope
            scopes_.emplace_back();

            // Build builtin lookup table
            for (size_t i = 0; i < num_builtins; ++i) {
                builtin_index_[builtins[i].name_] = i;
            }

            // Predefined constants
            define("pi", value::number(3.14159265358979323846));
            define("tau", value::number(6.28318530717958647692));
            define("euler", value::number(2.71828182845904523536));
            define("catalan", value::number(0.91596559417721901505));
            define("apery", value::number(1.20205690315959428540));
            define("phi", value::number(1.61803398874989484820)); // alias for golden_ratio
        }

        // ── Scope management ───────────────────────────────────────────────────
        void push_scope() {
            scopes_.emplace_back();
        }

        void pop_scope() {
            if (scopes_.size() > 1) {
                scopes_.pop_back();
            }
        }

        // ── Variable binding ───────────────────────────────────────────────────
        void define(const std::string& name, value val) {
            scopes_.back().bindings_[name] = std::move(val);
        }

        // ── Lookup: walk scope chain outward ───────────────────────────────────
        value* lookup(const std::string& name) {
            for (auto it = scopes_.rbegin(); it != scopes_.rend(); ++it) {
                auto found = it->bindings_.find(name);
                if (found != it->bindings_.end()) {
                    return &found->second;
                }
            }
            return nullptr;
        }

        // ── Built-in lookup ────────────────────────────────────────────────────
        const builtin* lookup_builtin(const std::string& name) const {
            auto it = builtin_index_.find(name);
            if (it != builtin_index_.end()) {
                return &builtins[it->second];
            }
            return nullptr;
        }

        // ── Module function lookup ─────────────────────────────────────────────
        const module::function_entry* lookup_module_fn(const std::string& name) const {
            if (module_registry_) {
                return module_registry_->lookup(name);
            }
            return nullptr;
        }
    };

} // --- idyl::core ---
