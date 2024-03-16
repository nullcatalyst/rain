#pragma once

#include <cstdint>

#include "rain/lang/serial/kind.hpp"

namespace rain::lang::serial {

struct LocalVariable {
    uint32_t type_id;
};

struct Variable {
    VariableKind kind;
    union {
        LocalVariable local;
    };
};

}  // namespace rain::lang::serial
