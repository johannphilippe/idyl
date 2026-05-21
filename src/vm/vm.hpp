#pragma once
#include <vector>
#include <memory>
#include <unordered_map>
#include <cstdint>

#include "vm/chunk.hpp"
#include "core/core.hpp"

// Forward-declare evaluator to break the circular include with evaluator.hpp.
namespace idyl::core { struct evaluator; }

namespace idyl::vm {

struct vm {
    // Set by evaluator after construction so that CALL_NATIVE and uncompiled
    // function fallbacks can call back into the evaluator.
    core::evaluator* eval_ = nullptr;

    // Compiled chunks, keyed by the same interned name_id_ as function_defs_.
    std::unordered_map<uint32_t, std::unique_ptr<bytecode_fn>> chunks_;

    // ── VM execution state ─────────────────────────────────────────────────
    struct call_frame {
        const bytecode_fn* fn;
        size_t             ip;   // instruction pointer (index into fn->code_)
        size_t             base; // start of this frame's slots in stack_
    };

    std::vector<core::value> stack_;
    std::vector<call_frame>  frames_;

    // Re-entrancy guard: the VM is not re-entrant.  When active_ is true,
    // eval_user_function falls back to the AST-walker instead of calling run().
    bool active_ = false;

    // ── Chunk registry ─────────────────────────────────────────────────────
    void store(uint32_t fn_id, std::unique_ptr<bytecode_fn> chunk) {
        chunks_[fn_id] = std::move(chunk);
    }

    bool has_compiled(uint32_t fn_id) const {
        return chunks_.count(fn_id) > 0;
    }

    const bytecode_fn* get_compiled(uint32_t fn_id) const {
        auto it = chunks_.find(fn_id);
        return it != chunks_.end() ? it->second.get() : nullptr;
    }

    // ── Run entry point ────────────────────────────────────────────────────
    // Execute the chunk for fn_id with the given positional args.
    // Assumes has_compiled(fn_id) is true and active_ is false.
    core::value run(uint32_t fn_id, const std::vector<core::value>& args);

    // Execute a reaction chunk (no params, all variable access via env).
    // Uses a local stack (independent of the main VM stack) so it does not
    // interfere with concurrent pure-function calls on the main stack.
    void run_reactions(const bytecode_fn* fn);

private:
    // Dispatch a CALL_NATIVE instruction: call builtins[b_idx] with `argc`
    // values from the top of the stack (left-most is deepest).
    core::value call_native(uint16_t b_idx, uint16_t argc);
};

} // namespace idyl::vm
