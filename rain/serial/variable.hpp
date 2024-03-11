#pragma once

#include <cstdint>

namespace rain::serial {

enum class VariableKind {
    Unknown,

    Local,
    Function,
    Export,

    Count,
};

struct LocalVariable {
    uint32_t type_id;
};

struct Variable {
    VariableKind kind;
    union {
        LocalVariable local;
    };
};

}  // namespace rain::serial
