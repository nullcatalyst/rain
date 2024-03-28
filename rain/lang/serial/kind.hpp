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
    StructLiteral,

    // Operations
    Ctor,
    Call,
    CompileTime,
    Member,
    UnaryOperator,
    BinaryOperator,
    Parenthesis,

    // Control flow
    Block,
    If,
    While,
    Return,

    // Declarations
    Export,
    Function,
    Let,
    Type,

    Count,
};

enum class UnaryOperatorKind {
    Unknown,

    // Arithmetic
    Positive,
    Negative,

    // Logical
    Not,

    // Bitwise
    BitwiseNot,

    Count,
};

enum class BinaryOperatorKind {
    Unknown,

    // Assignment
    Assign,

    // Arithmetic
    Add,
    Subtract,
    Multiply,
    Divide,
    Modulo,

    // Comparison
    Equal,
    NotEqual,
    Less,
    Greater,
    LessEqual,
    GreaterEqual,

    // Logical
    And,
    Or,
    Not,
    Xor,

    // Bitwise
    BitwiseAnd,
    BitwiseOr,
    BitwiseXor,
    BitwiseNot,
    LeftShift,
    RightShift,

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

    Unresolved,

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
