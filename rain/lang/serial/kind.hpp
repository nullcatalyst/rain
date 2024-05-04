#pragma once

namespace rain::lang::serial {

enum class ExpressionKind {
    Unknown,

    // Literals
    Null,
    Boolean,
    Integer,
    Float,
    String,
    Variable,
    Tuple,
    StructLiteral,
    ArrayLiteral,
    SliceLiteral,

    // Operations
    Call,
    CompileTime,
    Member,
    UnaryOperator,
    BinaryOperator,
    Parenthesis,
    Cast,

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

    UnwrappedOptional,

    Count,
};

}  // namespace rain::lang::serial
