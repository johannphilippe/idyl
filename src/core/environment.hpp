#pragma once

#include <unordered_map>
#include <vector>
#include <string>
#include <iostream>

#include "core/core.hpp"
#include "core/builtins.hpp"

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

        // ── Initialization ─────────────────────────────────────────────────────
        void init() {
            // Push global scope
            scopes_.emplace_back();

            // Build builtin lookup table
            for (size_t i = 0; i < num_builtins; ++i) {
                builtin_index_[builtins[i].name_] = i;
            }
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
    };

} // --- idyl::core ---
