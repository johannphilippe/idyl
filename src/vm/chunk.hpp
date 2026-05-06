#pragma once
#include <vector>
#include <string>
#include <cstdint>

#include "vm/instruction.hpp"
#include "core/core.hpp"

namespace idyl::vm {

// One compiled function — the unit of bytecode execution.
struct bytecode_fn {
    std::string  name_;
    uint32_t     name_id_     = 0; // interned name ID (matches function_defs_ key)
    uint16_t     param_count_ = 0; // number of positional parameters
    uint16_t     local_count_ = 0; // total slot count (params + block locals)

    std::vector<instruction>  code_;
    std::vector<core::value>  constants_; // constant pool

    // Add a constant, deduplicating numbers for compactness.
    uint16_t add_constant(core::value v) {
        if (v.type_ == core::value_t::number) {
            for (uint16_t i = 0; i < static_cast<uint16_t>(constants_.size()); ++i) {
                if (constants_[i].type_ == core::value_t::number &&
                    constants_[i].number_ == v.number_)
                    return i;
            }
        }
        constants_.push_back(std::move(v));
        return static_cast<uint16_t>(constants_.size() - 1);
    }

    // Disassemble to stderr (for debug / --vm-trace)
    void disassemble() const;
};

} // namespace idyl::vm
