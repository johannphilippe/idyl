#pragma once
#include <memory>
#include <string>
#include <unordered_map>

#include "vm/chunk.hpp"
#include "parser/ast.hpp"
#include "core/environment.hpp"

namespace idyl::vm {

// Compiles a pure user-function definition to a bytecode_fn.
// Returns nullptr when compilation is not possible (any unsupported AST node,
// named args, lambda blocks, etc.).  The evaluator falls back to AST-walking
// for any function that returns nullptr.
//
// Supported expression types (Step 3):
//   literals (number, time, trigger, string)
//   identifiers (params, numeric globals inlined as constants)
//   binary ops (+, -, *, /, %, <, <=, >, >=, ==, !=, &&, ||, +concat)
//   unary ops (-, !)
//   ternary (cond ? A ; B)
//   parenthesized
//   function calls to compiled user functions (by fn_id)
//   function calls to builtins (CALL_NATIVE)
//
struct compiler {
    // Compile def and return its chunk, or nullptr on failure.
    // env       — used to intern names and look up global numeric constants.
    // fn_defs   — the evaluator's function_defs_ table (uint32_t → ast ptr).
    std::unique_ptr<bytecode_fn> compile(
        const parser::function_definition& def,
        core::environment& env,
        const std::unordered_map<uint32_t,
            std::shared_ptr<parser::function_definition>>& fn_defs);

private:
    bytecode_fn*  chunk_    = nullptr;
    bool          failed_   = false;

    // Parameter / local slot map (name → slot index).
    std::unordered_map<std::string, uint16_t> slots_;
    uint16_t next_slot_ = 0;

    // Back-pointer to the fn_defs table and environment (set during compile()).
    const std::unordered_map<uint32_t,
        std::shared_ptr<parser::function_definition>>* fn_defs_ = nullptr;
    core::environment* env_ = nullptr;

    void fail() { failed_ = true; }

    void emit(instruction instr) { chunk_->code_.push_back(instr); }

    // Emit a placeholder jump and return the index for later patching.
    size_t emit_jump(opcode op) {
        size_t idx = chunk_->code_.size();
        emit({op, 0, 0, 0});
        return idx;
    }

    // Patch jump at instruction index `at` to the current code end.
    // Offset is relative to the instruction AFTER the jump (post-increment ip).
    void patch_jump(size_t at) {
        int32_t offset = static_cast<int32_t>(chunk_->code_.size())
                       - static_cast<int32_t>(at + 1);
        chunk_->code_[at].c = offset;
    }

    void compile_expr(const parser::expr_ptr& expr);
    // Compile a statement inside a block.
    // is_last: true when this is the final statement (its value becomes the block result).
    void compile_stmt(const parser::stmt_ptr& stmt, bool is_last);
};

} // namespace idyl::vm
