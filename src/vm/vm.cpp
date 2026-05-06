#include "vm/vm.hpp"
#include "core/evaluator.hpp"
#include "core/builtins.hpp"
#include "utilities/span.hpp"

#include <cmath>

namespace idyl::vm {

// ── call_native ───────────────────────────────────────────────────────────────

core::value vm::call_native(uint16_t b_idx, uint16_t argc) {
    // Collect argc args from the top of the stack (oldest = deepest)
    std::vector<core::value> args;
    args.reserve(argc);
    size_t base = stack_.size() - argc;
    for (size_t i = base; i < stack_.size(); ++i)
        args.push_back(std::move(stack_[i]));
    stack_.resize(base);

    const core::builtin& bi = core::builtins[b_idx];
    span<const core::value> sp{args.data(), args.size()};
    return bi.function_(sp);
}

// ── run ──────────────────────────────────────────────────────────────────────

core::value vm::run(uint32_t fn_id, const std::vector<core::value>& init_args) {
    active_ = true;

    const bytecode_fn* root = get_compiled(fn_id);
    if (!root) { active_ = false; return core::value::nil(); }

    // Pre-allocate stack space to avoid frequent reallocations.
    if (stack_.capacity() < 256) stack_.reserve(256);
    if (frames_.capacity() < 64) frames_.reserve(64);

    // Push initial args then the root frame
    size_t base = stack_.size();
    for (const auto& a : init_args) stack_.push_back(a);
    // Grow for any locals beyond params (Step 3: local_count == param_count)
    if (root->local_count_ > root->param_count_) {
        stack_.resize(base + root->local_count_, core::value::nil());
    }
    frames_.push_back({root, 0, base});

    // ── Main dispatch loop ────────────────────────────────────────────────
    // We access frame fields by index rather than reference so that
    // frames_.push_back() in CALL (which may reallocate) never leaves us
    // with a dangling reference.
    while (!frames_.empty()) {
        // Snapshot the fields we need from the current frame before any
        // push_back / pop_back that could invalidate pointers.
        const size_t fi = frames_.size() - 1;
        const bytecode_fn* fn   = frames_[fi].fn;
        const size_t       base = frames_[fi].base;
        const instruction  instr = fn->code_[frames_[fi].ip++];

        switch (instr.op) {

        case opcode::LOAD_CONST:
            stack_.push_back(fn->constants_[instr.a]);
            break;

        case opcode::LOAD_LOCAL:
            stack_.push_back(stack_[base + instr.a]);
            break;

        case opcode::STORE_LOCAL:
            stack_[base + instr.a] = std::move(stack_.back());
            stack_.pop_back();
            break;

        case opcode::LOAD_NIL:
            stack_.push_back(core::value::nil());
            break;

        case opcode::POP:
            stack_.pop_back();
            break;

        // ── Arithmetic ───────────────────────────────────────────────────
        case opcode::ADD: {
            double b = stack_.back().as_number(); stack_.pop_back();
            double a = stack_.back().as_number(); stack_.pop_back();
            stack_.push_back(core::value::number(a + b));
            break;
        }
        case opcode::SUB: {
            double b = stack_.back().as_number(); stack_.pop_back();
            double a = stack_.back().as_number(); stack_.pop_back();
            stack_.push_back(core::value::number(a - b));
            break;
        }
        case opcode::MUL: {
            double b = stack_.back().as_number(); stack_.pop_back();
            double a = stack_.back().as_number(); stack_.pop_back();
            stack_.push_back(core::value::number(a * b));
            break;
        }
        case opcode::DIV: {
            double b = stack_.back().as_number(); stack_.pop_back();
            double a = stack_.back().as_number(); stack_.pop_back();
            stack_.push_back(core::value::number(b != 0.0 ? a / b : 0.0));
            break;
        }
        case opcode::MOD: {
            double b = stack_.back().as_number(); stack_.pop_back();
            double a = stack_.back().as_number(); stack_.pop_back();
            stack_.push_back(core::value::number(b != 0.0 ? std::fmod(a, b) : 0.0));
            break;
        }

        // ── Unary ────────────────────────────────────────────────────────
        case opcode::NEG: {
            double a = stack_.back().as_number(); stack_.pop_back();
            stack_.push_back(core::value::number(-a));
            break;
        }
        case opcode::NOT_OP: {
            bool a = stack_.back().is_truthy(); stack_.pop_back();
            stack_.push_back(core::value::trigger(!a));
            break;
        }

        // ── Comparisons ──────────────────────────────────────────────────
        case opcode::LT: {
            double b = stack_.back().as_number(); stack_.pop_back();
            double a = stack_.back().as_number(); stack_.pop_back();
            stack_.push_back(core::value::trigger(a < b));
            break;
        }
        case opcode::LE: {
            double b = stack_.back().as_number(); stack_.pop_back();
            double a = stack_.back().as_number(); stack_.pop_back();
            stack_.push_back(core::value::trigger(a <= b));
            break;
        }
        case opcode::GT: {
            double b = stack_.back().as_number(); stack_.pop_back();
            double a = stack_.back().as_number(); stack_.pop_back();
            stack_.push_back(core::value::trigger(a > b));
            break;
        }
        case opcode::GE: {
            double b = stack_.back().as_number(); stack_.pop_back();
            double a = stack_.back().as_number(); stack_.pop_back();
            stack_.push_back(core::value::trigger(a >= b));
            break;
        }
        case opcode::EQ: {
            core::value b = std::move(stack_.back()); stack_.pop_back();
            core::value a = std::move(stack_.back()); stack_.pop_back();
            stack_.push_back(core::value::trigger(a.as_number() == b.as_number()));
            break;
        }
        case opcode::NE: {
            core::value b = std::move(stack_.back()); stack_.pop_back();
            core::value a = std::move(stack_.back()); stack_.pop_back();
            stack_.push_back(core::value::trigger(a.as_number() != b.as_number()));
            break;
        }

        // ── Logical ──────────────────────────────────────────────────────
        case opcode::AND: {
            bool b = stack_.back().is_truthy(); stack_.pop_back();
            bool a = stack_.back().is_truthy(); stack_.pop_back();
            stack_.push_back(core::value::trigger(a && b));
            break;
        }
        case opcode::OR: {
            bool b = stack_.back().is_truthy(); stack_.pop_back();
            bool a = stack_.back().is_truthy(); stack_.pop_back();
            stack_.push_back(core::value::trigger(a || b));
            break;
        }

        // ── String concat ─────────────────────────────────────────────────
        case opcode::CONCAT: {
            std::string b = stack_.back().as_string(); stack_.pop_back();
            std::string a = stack_.back().as_string(); stack_.pop_back();
            stack_.push_back(core::value::string(a + b));
            break;
        }

        // ── Control flow ──────────────────────────────────────────────────
        case opcode::JUMP:
            frames_[fi].ip += instr.c;
            break;

        case opcode::JUMP_IF_FALSY: {
            bool truthy = stack_.back().is_truthy();
            stack_.pop_back();
            if (!truthy) frames_[fi].ip += instr.c;
            break;
        }

        // ── CALL: push a new frame for a compiled user function ───────────
        case opcode::CALL: {
            uint32_t callee_id = static_cast<uint32_t>(instr.c);
            uint16_t argc      = instr.a;

            const bytecode_fn* callee = get_compiled(callee_id);
            if (callee) {
                // Args are already on the stack — their base is the new frame base.
                size_t new_base = stack_.size() - argc;
                if (callee->local_count_ > callee->param_count_) {
                    stack_.resize(new_base + callee->local_count_, core::value::nil());
                }
                frames_.push_back({callee, 0, new_base});
                // The loop will now dispatch from the new frame next iteration.
            } else {
                // Callee is not compiled — fall back to evaluator AST-walker.
                // Collect args from the stack.
                std::vector<core::value> args;
                args.reserve(argc);
                size_t arg_base = stack_.size() - argc;
                for (size_t i = arg_base; i < stack_.size(); ++i)
                    args.push_back(std::move(stack_[i]));
                stack_.resize(arg_base);

                core::value result = core::value::nil();
                if (eval_) {
                    // Temporarily release the active_ guard so the evaluator
                    // can call back into the VM for other compiled functions.
                    // (Safe because this call frame is already on frames_.)
                    active_ = false;
                    auto it = eval_->function_defs_.find(callee_id);
                    if (it != eval_->function_defs_.end() && it->second)
                        result = eval_->eval_user_function(*it->second, args);
                    active_ = true;
                }
                stack_.push_back(std::move(result));
            }
            break;
        }

        // ── CALL_NATIVE: call a builtin function ──────────────────────────
        case opcode::CALL_NATIVE: {
            core::value result = call_native(instr.b, instr.a);
            stack_.push_back(std::move(result));
            break;
        }

        // ── RETURN ────────────────────────────────────────────────────────
        case opcode::RETURN: {
            core::value result = std::move(stack_.back());
            stack_.pop_back();
            // Restore the stack to before this frame's locals/args
            stack_.resize(base);
            frames_.pop_back();

            if (frames_.empty()) {
                // Root function returned — we're done
                active_ = false;
                return result;
            }
            // Push result for the calling frame
            stack_.push_back(std::move(result));
            break;
        }

        case opcode::NOP:
            break;

        default:
            // Unknown opcode — abort
            stack_.resize(frames_.front().base);
            frames_.clear();
            active_ = false;
            return core::value::nil();
        }
    }

    // Fell off the end without RETURN — shouldn't happen in well-formed code
    active_ = false;
    return core::value::nil();
}

} // namespace idyl::vm
