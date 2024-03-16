#pragma once

#include <cstdint>

#include "rain/lang/serial/kind.hpp"

namespace rain::lang::serial {

struct BuiltinType {
    uint32_t type_id;
};

struct FunctionType {
    uint32_t return_type_id;
    uint32_t parameter_type_start;
    uint32_t parameter_type_end;
};

struct OptionalType {
    uint32_t type_id;
};

struct PointerType {
    uint32_t type_id;
};

struct ArrayType {
    uint32_t type_id;
    uint32_t size;
};

struct SliceType {
    uint32_t type_id;
};

struct TupleType {
    uint32_t type_start;
    uint32_t type_end;
};

struct AliasType {
    uint32_t type_id;
};

struct StructType {
    uint32_t field_start;
    uint32_t field_end;
};

struct InterfaceType {};

////////////////////////////////
// Type
////////////////////////////////

struct Type {
    TypeKind kind;
    union {
        BuiltinType   builtin;
        FunctionType  function;
        OptionalType  optional;
        PointerType   pointer;
        ArrayType     array;
        SliceType     slice;
        TupleType     tuple;
        AliasType     alias;
        StructType    struct_;
        InterfaceType interface;
    };
};

}  // namespace rain::lang::serial
