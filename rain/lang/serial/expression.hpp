#pragma once

#include <cstdint>
#include <memory>
#include <string_view>

#include "rain/crypto/sha256.hpp"
#include "rain/err/simple.hpp"
#include "rain/util/result.hpp"

namespace rain::lang::serial {

////////////////////////////////////////////////////////////////
// Expressions
////////////////////////////////////////////////////////////////

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

////////////////////////////////
// Literals
////////////////////////////////

struct IntegerExpression {
    uint64_t value;
};

struct FloatExpression {
    double value;
};

struct VariableExpression {
    uint32_t variable_id;
};

struct TupleExpression {
    uint32_t expressions_start;
    uint32_t expressions_end;
};

////////////////////////////////
// Operations
////////////////////////////////

struct CtorExpression {
    uint32_t type_id;
    uint32_t arguments_start;
    uint32_t arguments_end;
};

struct CallExpression {
    uint32_t expression_id;
    uint32_t arguments_start;
    uint32_t arguments_end;
};

struct ExecExpression {
    uint32_t expression_id;
};

struct MemberExpression {
    uint32_t expression_id;
    uint32_t member_string_start;
    uint32_t member_string_end;
};

struct UnaryOperatorExpression {
    uint32_t expression_id;
    uint32_t operator_id;
};

struct BinaryOperatorExpression {
    uint32_t lhs_id;
    uint32_t rhs_id;
    uint32_t operator_id;
};

struct ParenthesisExpression {
    uint32_t expression_id;
};

////////////////////////////////
// Control flow
////////////////////////////////

struct BlockExpression {
    uint32_t expressions_start;
    uint32_t expressions_end;
};

struct FunctionExpression {
    uint32_t type_id;        // FunctionType
    uint32_t parameters_id;  // Tuple
    uint32_t body_id;        // Block
};

struct IfExpression {
    uint32_t condition_id;
    uint32_t then_id;
    uint32_t else_id;
};

struct ReturnExpression {};

////////////////////////////////
// Declarations
////////////////////////////////

struct ExportExpression {};

struct LetExpression {};

////////////////////////////////
// Expression
////////////////////////////////

struct Expression {
    ExpressionKind kind;
    union {
        IntegerExpression        integer;
        FloatExpression          float_;
        VariableExpression       variable;
        CtorExpression           ctor;
        CallExpression           call;
        ExecExpression           exec;
        MemberExpression         member;
        UnaryOperatorExpression  unary_operator;
        BinaryOperatorExpression binary_operator;
        ParenthesisExpression    parenthesis;
        BlockExpression          block;
        FunctionExpression       function;
        IfExpression             if_;
        ReturnExpression         return_;
        ExportExpression         export_;
        LetExpression            let;
    };
};

}  // namespace rain::lang::serial
