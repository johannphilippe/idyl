#pragma once

#include <unordered_map>
#include <vector>
#include <string>
#include <iostream>
#include <cstdint>

#include "core/core.hpp"
#include "core/builtins.hpp"
#include "include/module.hpp"

namespace idyl::core {

    // ── String intern table ────────────────────────────────────────────────────
    // Maps each distinct string to a uint32_t ID starting from 1.
    // ID 0 is reserved as "not yet interned" (sentinel for lazy caching in AST).
    // Lives inside environment so it is created/destroyed with the evaluator.
    struct string_intern {
        std::unordered_map<std::string, uint32_t> map_;
        std::vector<std::string> strings_; // strings_[id-1] == the string

        // Return the existing ID for s, or allocate a new one.
        uint32_t intern(const std::string& s) {
            auto it = map_.find(s);
            if (it != map_.end()) return it->second;
            uint32_t id = static_cast<uint32_t>(strings_.size()) + 1;
            strings_.push_back(s);
            map_.emplace(s, id);
            return id;
        }

        // Try to find the ID for s without allocating. Returns 0 if not found.
        uint32_t find(const std::string& s) const {
            auto it = map_.find(s);
            return it != map_.end() ? it->second : 0;
        }

        const std::string& str(uint32_t id) const { return strings_[id - 1]; }
    };

    // ── Runtime scope frame ────────────────────────────────────────────────────
    // Keys are interned string IDs — integer hashing is ~10× faster than
    // std::string hashing, and the key size drops from 16–32 bytes to 4 bytes.
    struct scope_frame {
        std::unordered_map<uint32_t, value> bindings_;
    };

    // ── Runtime environment ────────────────────────────────────────────────────
    // Manages the scope chain and built-in function lookup for the evaluator.
    struct environment {

        // Scope chain: back() is innermost (current) scope
        std::vector<scope_frame> scopes_;

        // ── Scope pinning for process callbacks ────────────────────────────────
        // When a scheduler callback fires for a process, these two fields isolate
        // that process's scope from sibling processes that share the scope stack.
        //
        // scope_pin_:   index of the active process's scope frame (0 = no pin).
        // scope_start_: number of scopes at the start of the callback, so that
        //               scopes pushed DURING the callback (for user function calls)
        //               are recognized as local rather than sibling-process scopes.
        //
        // With these set, lookup() skips scopes at indices in (scope_pin_, scope_start_)
        // (those are sibling-process scopes), and define() writes to scope_pin_ rather
        // than scopes_.back() when no local sub-scope has been pushed yet.
        size_t scope_pin_   = 0;
        size_t scope_start_ = 0;

        // Intern table — shared across the entire evaluator lifetime.
        string_intern intern_;

        // Built-in function lookup (interned name ID → index into core::builtins[])
        std::unordered_map<uint32_t, size_t> builtin_index_;

        // Module registry (owned externally — populated before init)
        module::registry* module_registry_ = nullptr;

        // ── Intern helper ──────────────────────────────────────────────────────
        uint32_t intern(const std::string& s) { return intern_.intern(s); }

        // ── Initialization ─────────────────────────────────────────────────────
        void init() {
            // Push global scope
            scopes_.emplace_back();

            // Build builtin lookup table with interned keys
            for (size_t i = 0; i < num_builtins; ++i) {
                uint32_t id = intern_.intern(builtins[i].name_);
                builtin_index_[id] = i;
            }

            // Predefined constants
            define("pi",      value::number(3.14159265358979323846));
            define("tau",     value::number(6.28318530717958647692));
            define("euler",   value::number(2.71828182845904523536));
            define("catalan", value::number(0.91596559417721901505));
            define("apery",   value::number(1.20205690315959428540));
            define("phi",     value::number(1.61803398874989484820));
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

        // ── Variable binding (string key — interns on first call) ──────────────
        void define(const std::string& name, value val) {
            uint32_t id = intern_.intern(name);
            scopes_[define_target_()].bindings_[id] = std::move(val);
        }

        // ── Variable binding (pre-interned key — zero extra hashing) ──────────
        void define(uint32_t id, value val) {
            scopes_[define_target_()].bindings_[id] = std::move(val);
        }

        // ── Lookup by pre-interned ID (fast path) ─────────────────────────────
        value* lookup(uint32_t id) {
            for (int i = static_cast<int>(scopes_.size()) - 1; i >= 0; --i) {
                // When pinned, skip sibling-process scopes: those are frames above
                // scope_pin_ that existed before this callback started (i.e. indices
                // in the open interval (scope_pin_, scope_start_)).
                if (scope_pin_ > 0
                        && static_cast<size_t>(i) > scope_pin_
                        && static_cast<size_t>(i) < scope_start_)
                    continue;
                auto found = scopes_[i].bindings_.find(id);
                if (found != scopes_[i].bindings_.end()) return &found->second;
            }
            return nullptr;
        }

        // ── Lookup by string (interns internally; use sparingly on hot path) ───
        value* lookup(const std::string& name) {
            uint32_t id = intern_.find(name);
            if (id == 0) return nullptr; // never defined → not in any scope
            return lookup(id);
        }

        // ── Built-in lookup by pre-interned ID ────────────────────────────────
        const builtin* lookup_builtin(uint32_t id) const {
            auto it = builtin_index_.find(id);
            return it != builtin_index_.end() ? &builtins[it->second] : nullptr;
        }

        // ── Built-in lookup by string ─────────────────────────────────────────
        const builtin* lookup_builtin(const std::string& name) const {
            uint32_t id = intern_.find(name);
            if (id == 0) return nullptr;
            return lookup_builtin(id);
        }

        // ── Module function lookup ─────────────────────────────────────────────
        const module::function_entry* lookup_module_fn(const std::string& name) const {
            if (module_registry_) {
                return module_registry_->lookup(name);
            }
            return nullptr;
        }

    private:
        // When pinned with no local sub-scopes yet (scopes_.size() == scope_start_),
        // write to the process scope (scope_pin_) rather than the innermost frame,
        // which would be a sibling-process scope.  Once sub-scopes are pushed during
        // a callback (scopes_.size() > scope_start_), write to the innermost as usual.
        size_t define_target_() const {
            if (scope_pin_ > 0 && scope_start_ > 0
                    && scopes_.size() == scope_start_
                    && scope_pin_ < scopes_.size())
                return scope_pin_;
            return scopes_.size() - 1;
        }
    };

} // --- idyl::core ---
