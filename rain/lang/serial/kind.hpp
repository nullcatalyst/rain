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
    String,
    Variable,
    Tuple,
    StructLiteral,
    ArrayLiteral,

    // Operations
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
    Extern,
    FunctionDeclaration,
    Function,
    Method,
    Let,
    Type,
    Implementation,

    Count,
};

enum class UnaryOperatorKind {
    Unknown,

    // Arithmetic
    Positive,
    Negative,

    // Logical
    Not,

    // Other
    Reference,
    HasValue,

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
    Remainder,

    // Comparison
    Equal,
    NotEqual,
    Less,
    Greater,
    LessEqual,
    GreaterEqual,

    // Bitwise
    And,
    Or,
    Xor,
    ShiftLeft,
    ShiftRight,
    RotateLeft,
    RotateRight,

    // Conversion
    CastFrom,
    ArrayIndex,

    Count,
};

enum class TypeKind {
    Unknown,

    Null,
    Builtin,

    Function,
    Optional,
    Reference,
    Array,
    Slice,
    Tuple,

    Alias,
    Struct,
    Interface,

    Meta,
    Unresolved,

    Count,
};

enum class VariableKind {
    Unknown,

    Global,
    Local,
    Function,

    Count,
};

}  // namespace rain::lang::serial
