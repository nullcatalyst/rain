#pragma once

#include "cirrus/ast/expr/expression.hpp"

namespace cirrus::ast {

enum class BinaryOperator {
    Unknown,
    Add,
    Subtract,
    Multiply,
    Divide,
    Modulo,
    And,
    Or,
    Xor,
    ShiftLeft,
    ShiftRight,
    Equal,
    NotEqual,
    Less,
    Greater,
    LessEqual,
    GreaterEqual,
};

struct BinaryOperatorExpressionData : public ExpressionData {
    Expression     lhs;
    Expression     rhs;
    BinaryOperator op;
};

struct BinaryOperatorExpression
    : public IExpression<BinaryOperatorExpression, ExpressionVtbl, BinaryOperatorExpressionData> {
    using IExpression<BinaryOperatorExpression, ExpressionVtbl,
                      BinaryOperatorExpressionData>::IExpression;

    static const ExpressionVtbl _vtbl;

    [[nodiscard]] static bool is(const Expression expr) noexcept { return expr.vtbl() == &_vtbl; }
    [[nodiscard]] static BinaryOperatorExpression alloc(Expression lhs, Expression rhs,
                                                        BinaryOperator op) noexcept;

    [[nodiscard]] constexpr const Expression& lhs() const noexcept { return _data->lhs; }
    [[nodiscard]] constexpr const Expression& rhs() const noexcept { return _data->rhs; }
    [[nodiscard]] constexpr BinaryOperator    op() const noexcept { return _data->op; }
};

}  // namespace cirrus::ast
