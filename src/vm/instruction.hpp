#pragma once
#include <cstdint>

namespace idyl::vm {

enum class opcode : uint8_t {
    // Loads / stores
    LOAD_CONST,     // a=const_idx  → push constants[a]
    LOAD_LOCAL,     // a=slot       → push stack[base+a]
    STORE_LOCAL,    // a=slot       → pop into stack[base+a]
    LOAD_NIL,       //              → push nil
    POP,            //              → discard top of stack

    // Arithmetic (pop 2, push 1)
    ADD, SUB, MUL, DIV, MOD,

    // Unary (pop 1, push 1)
    NEG, NOT_OP,

    // Comparisons (pop 2, push trigger)
    LT, LE, GT, GE, EQ, NE,

    // Logical (pop 2, push trigger)
    AND, OR,

    // String concat (pop 2, push string)
    CONCAT,

    // Control flow
    JUMP,           // c=offset (signed, relative to instruction after this one)
    JUMP_IF_FALSY,  // c=offset; pop and test condition

    // Calls
    CALL,           // a=argc, c=fn_id — call compiled user function
    CALL_NATIVE,    // a=argc, b=builtin_idx — call builtin function

    // Return
    RETURN,         // pop top, return to caller

    NOP,
};

struct instruction {
    opcode   op  = opcode::NOP;
    uint16_t a   = 0;   // slot / const-index / argc
    uint16_t b   = 0;   // builtin_idx / reserved
    int32_t  c   = 0;   // fn_id (CALL) / jump offset / reserved
};

inline const char* opcode_name(opcode op) {
    switch (op) {
        case opcode::LOAD_CONST:    return "LOAD_CONST";
        case opcode::LOAD_LOCAL:    return "LOAD_LOCAL";
        case opcode::STORE_LOCAL:   return "STORE_LOCAL";
        case opcode::LOAD_NIL:      return "LOAD_NIL";
        case opcode::POP:           return "POP";
        case opcode::ADD:           return "ADD";
        case opcode::SUB:           return "SUB";
        case opcode::MUL:           return "MUL";
        case opcode::DIV:           return "DIV";
        case opcode::MOD:           return "MOD";
        case opcode::NEG:           return "NEG";
        case opcode::NOT_OP:        return "NOT";
        case opcode::LT:            return "LT";
        case opcode::LE:            return "LE";
        case opcode::GT:            return "GT";
        case opcode::GE:            return "GE";
        case opcode::EQ:            return "EQ";
        case opcode::NE:            return "NE";
        case opcode::AND:           return "AND";
        case opcode::OR:            return "OR";
        case opcode::CONCAT:        return "CONCAT";
        case opcode::JUMP:          return "JUMP";
        case opcode::JUMP_IF_FALSY: return "JUMP_IF_FALSY";
        case opcode::CALL:          return "CALL";
        case opcode::CALL_NATIVE:   return "CALL_NATIVE";
        case opcode::RETURN:        return "RETURN";
        default:                    return "NOP";
    }
}

} // namespace idyl::vm
