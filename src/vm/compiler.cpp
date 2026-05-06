#include "vm/compiler.hpp"
#include "core/builtins.hpp"

#include <cmath>

namespace idyl::vm {

// ── Time literal → milliseconds ───────────────────────────────────────────────
static double time_literal_to_ms(const std::string& val, const std::string& unit,
                                  double main_bpm = 120.0) {
    double v = std::stod(val);
    if (unit == "ms")  return v;
    if (unit == "s")   return v * 1000.0;
    if (unit == "m")   return v * 60000.0;
    if (unit == "hz")  return (v > 0.0) ? 1000.0 / v : 0.0;
    if (unit == "bpm") return (v > 0.0) ? 60000.0 / v : 0.0;
    if (unit == "b")   return v * (60000.0 / main_bpm);
    return v;
}

// ── compile_expr ─────────────────────────────────────────────────────────────

void compiler::compile_expr(const parser::expr_ptr& expr) {
    if (!expr || failed_) return;

    switch (expr->type_) {

    // ── Literal ────────────────────────────────────────────────────────────
    case parser::node_t::literal_expr: {
        auto& le = static_cast<const parser::literal_expr&>(*expr);
        if (!le.literal_) { fail(); return; }

        core::value cv;
        switch (le.literal_->type_) {
            case parser::node_t::number_literal: {
                auto& nl = static_cast<const parser::number_literal&>(*le.literal_);
                cv = core::value::number(std::stod(nl.value_));
                break;
            }
            case parser::node_t::time_literal: {
                auto& tl = static_cast<const parser::time_literal&>(*le.literal_);
                cv = core::value::time_ms(time_literal_to_ms(tl.value_, tl.unit_));
                break;
            }
            case parser::node_t::trigger_literal:
                cv = core::value::trigger(true);
                break;
            case parser::node_t::rest_literal:
                cv = core::value::trigger(false);
                break;
            case parser::node_t::string_literal: {
                auto& sl = static_cast<const parser::string_literal&>(*le.literal_);
                cv = core::value::string(sl.value_);
                break;
            }
            default:
                fail(); return;
        }
        uint16_t ci = chunk_->add_constant(std::move(cv));
        emit({opcode::LOAD_CONST, ci});
        break;
    }

    // ── Identifier ─────────────────────────────────────────────────────────
    case parser::node_t::identifier_expr: {
        auto& ie = static_cast<const parser::identifier_expr&>(*expr);
        if (!ie.identifier_) { fail(); return; }
        const std::string& name = ie.identifier_->name_;

        // Local parameter / block variable → LOAD_LOCAL
        auto sit = slots_.find(name);
        if (sit != slots_.end()) {
            emit({opcode::LOAD_LOCAL, sit->second});
            return;
        }

        // Global numeric constant (pi, tau, etc.) → inline as LOAD_CONST
        if (auto* v = env_->lookup(name)) {
            if (v->type_ == core::value_t::number || v->type_ == core::value_t::time) {
                uint16_t ci = chunk_->add_constant(*v);
                emit({opcode::LOAD_CONST, ci});
                return;
            }
        }

        // Anything else (function refs, flows, etc.) — not supported yet
        fail();
        break;
    }

    // ── Binary operator ────────────────────────────────────────────────────
    case parser::node_t::binary_op_expr: {
        auto& boe = static_cast<const parser::binary_op_expr&>(*expr);
        if (!boe.op_) { fail(); return; }
        compile_expr(boe.op_->left_);
        compile_expr(boe.op_->right_);
        if (failed_) return;

        const std::string& op = boe.op_->op_;
        if      (op == "+")  emit({opcode::ADD});
        else if (op == "-")  emit({opcode::SUB});
        else if (op == "*")  emit({opcode::MUL});
        else if (op == "/")  emit({opcode::DIV});
        else if (op == "%")  emit({opcode::MOD});
        else if (op == "<")  emit({opcode::LT});
        else if (op == "<=") emit({opcode::LE});
        else if (op == ">")  emit({opcode::GT});
        else if (op == ">=") emit({opcode::GE});
        else if (op == "==") emit({opcode::EQ});
        else if (op == "!=") emit({opcode::NE});
        else if (op == "&&") emit({opcode::AND});
        else if (op == "||") emit({opcode::OR});
        else if (op == "++") emit({opcode::CONCAT});
        else { fail(); return; }
        break;
    }

    // ── Unary operator ─────────────────────────────────────────────────────
    case parser::node_t::unary_op_expr: {
        auto& uoe = static_cast<const parser::unary_op_expr&>(*expr);
        if (!uoe.op_) { fail(); return; }
        compile_expr(uoe.op_->operand_);
        if (failed_) return;

        const std::string& op = uoe.op_->op_;
        if      (op == "-") emit({opcode::NEG});
        else if (op == "!") emit({opcode::NOT_OP});
        else { fail(); return; }
        break;
    }

    // ── Ternary (cond ? A ; B) ─────────────────────────────────────────────
    // Idyl ternary: condition.as_number() is an index into options_[].
    // For a trigger condition: false→0→options_[0], true→1→options_[1].
    // Only the binary (N=2) case is compiled; N≠2 falls back to AST-walker.
    case parser::node_t::ternary_op_expr: {
        auto& toe = static_cast<const parser::ternary_op_expr&>(*expr);
        if (!toe.op_ || toe.op_->options_.size() != 2) { fail(); return; }

        compile_expr(toe.op_->condition_);
        if (failed_) return;

        // JUMP_IF_FALSY: condition false (idx=0) → jump to options_[0]
        // Fall-through:  condition true  (idx=1) → execute options_[1]
        size_t jf = emit_jump(opcode::JUMP_IF_FALSY);

        compile_expr(toe.op_->options_[1]); // truthy / idx=1
        if (failed_) return;

        size_t jmp = emit_jump(opcode::JUMP);
        patch_jump(jf);

        compile_expr(toe.op_->options_[0]); // falsy / idx=0
        if (failed_) return;

        patch_jump(jmp);
        break;
    }

    // ── Parenthesized ──────────────────────────────────────────────────────
    case parser::node_t::parenthesized_expr: {
        auto& pe = static_cast<const parser::parenthesized_expr&>(*expr);
        compile_expr(pe.expr_);
        break;
    }

    // ── Function call ──────────────────────────────────────────────────────
    case parser::node_t::function_call_expr: {
        auto& fce = static_cast<const parser::function_call_expr&>(*expr);
        if (!fce.call_ || !fce.call_->function_) { fail(); return; }

        // Only simple identifier calls (no module access, no indirect)
        if (fce.call_->function_->type_ != parser::node_t::identifier_expr) {
            fail(); return;
        }
        auto& id = static_cast<const parser::identifier_expr&>(*fce.call_->function_);
        if (!id.identifier_) { fail(); return; }
        const std::string& fn_name = id.identifier_->name_;

        // No named arguments in Step 3
        for (const auto& arg : fce.call_->arguments_) {
            if (arg && !arg->name_.empty()) { fail(); return; }
        }

        // Compile each positional argument onto the stack
        uint16_t argc = 0;
        for (const auto& arg : fce.call_->arguments_) {
            if (!arg || !arg->value_) { fail(); return; }
            compile_expr(arg->value_);
            if (failed_) return;
            ++argc;
        }

        // Is it a compiled user function?
        uint32_t fn_id = env_->intern(fn_name);
        if (fn_defs_->count(fn_id)) {
            emit({opcode::CALL, argc, 0, static_cast<int32_t>(fn_id)});
            return;
        }

        // Is it a builtin?
        if (uint32_t bid = env_->intern_.find(fn_name)) {
            if (const core::builtin* bi = env_->lookup_builtin(bid)) {
                // Find the builtin index
                for (size_t i = 0; i < core::num_builtins; ++i) {
                    if (core::builtins[i].name_ == fn_name) {
                        emit({opcode::CALL_NATIVE, argc,
                              static_cast<uint16_t>(i), 0});
                        return;
                    }
                }
            }
        }

        // Unknown function — fail compilation
        fail();
        break;
    }

    // ── Everything else: not supported in Step 3 ───────────────────────────
    default:
        fail();
        break;
    }
}

// ── compile ──────────────────────────────────────────────────────────────────

std::unique_ptr<bytecode_fn> compiler::compile(
    const parser::function_definition& def,
    core::environment& env,
    const std::unordered_map<uint32_t,
        std::shared_ptr<parser::function_definition>>& fn_defs)
{
    if (!def.body_)         return nullptr;
    if (def.lambda_block_)  return nullptr; // temporal — not compilable

    // Reset compiler state
    failed_    = false;
    slots_.clear();
    next_slot_ = 0;
    fn_defs_   = &fn_defs;
    env_       = &env;

    auto fn = std::make_unique<bytecode_fn>();
    fn->name_        = def.name_;
    fn->name_id_     = env.intern(def.name_);
    fn->param_count_ = static_cast<uint16_t>(def.parameters_.size());

    chunk_ = fn.get();

    // Assign parameter slots (slot 0 = first param, etc.)
    for (const auto& p : def.parameters_) {
        if (!p) { chunk_ = nullptr; return nullptr; }
        slots_[p->name_] = next_slot_++;
    }
    fn->local_count_ = next_slot_; // Step 3: locals == params

    // Compile body
    compile_expr(def.body_);

    if (failed_) { chunk_ = nullptr; return nullptr; }

    emit({opcode::RETURN});
    chunk_ = nullptr;
    return fn;
}

// ── disassemble ───────────────────────────────────────────────────────────────

void bytecode_fn::disassemble() const {
    std::fprintf(stderr, "=== %s (params=%u locals=%u) ===\n",
                 name_.c_str(), param_count_, local_count_);
    for (size_t i = 0; i < code_.size(); ++i) {
        const auto& ins = code_[i];
        std::fprintf(stderr, "  %4zu  %-16s", i, opcode_name(ins.op));
        switch (ins.op) {
            case opcode::LOAD_CONST:
                std::fprintf(stderr, " k[%u] (= %s)", ins.a,
                             constants_[ins.a].as_string().c_str());
                break;
            case opcode::LOAD_LOCAL:
            case opcode::STORE_LOCAL:
                std::fprintf(stderr, " slot[%u]", ins.a);
                break;
            case opcode::JUMP:
            case opcode::JUMP_IF_FALSY:
                std::fprintf(stderr, " +%d  → %zu", ins.c, i + 1 + ins.c);
                break;
            case opcode::CALL:
                std::fprintf(stderr, " fn_id=%d argc=%u", ins.c, ins.a);
                break;
            case opcode::CALL_NATIVE:
                std::fprintf(stderr, " builtin[%u] argc=%u", ins.b, ins.a);
                break;
            default:
                break;
        }
        std::fprintf(stderr, "\n");
    }
}

} // namespace idyl::vm
