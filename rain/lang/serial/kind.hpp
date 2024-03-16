#pragma once

namespace rain::lang::serial {

enum class ExpressionKind {
    Unknown,

    // Literals
    Null,
    True,
    False,
    Integer,
    Float,
    Variable,
    Tuple,

    // Operations
    Ctor,
    Call,
    Exec,
    Member,
    UnaryOperator,
    BinaryOperator,
    Parenthesis,

    // Control flow
    Block,
    If,
    Return,

    // Declarations
    Function,
    Let,

    Count,
};

enum class TypeKind {
    Unknown,

    Null,
    Builtin,

    Function,
    Optional,
    Pointer,
    Array,
    Slice,
    Tuple,

    Alias,
    Struct,
    Interface,

    Count,
};

enum class VariableKind {
    Unknown,

    Local,
    Function,
    Export,

    Count,
};

}  // namespace rain::lang::serial