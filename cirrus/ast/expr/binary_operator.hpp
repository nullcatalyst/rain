#pragma once

#include "cirrus/ast/expr/expression.hpp"

namespace cirrus::ast {

enum class BinaryOperatorKind {
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

class BinaryOperatorExpression : public Expression {
    ExpressionPtr      _lhs;
    ExpressionPtr      _rhs;
    BinaryOperatorKind _op_kind;

  public:
    BinaryOperatorExpression(ExpressionPtr lhs, ExpressionPtr rhs,
                             BinaryOperatorKind op_kind) noexcept
        : _lhs(std::move(lhs)), _rhs(std::move(rhs)), _op_kind(op_kind) {}

    [[nodiscard]] static std::shared_ptr<BinaryOperatorExpression> alloc(
        ExpressionPtr lhs, ExpressionPtr rhs, BinaryOperatorKind op_kind) {
        return std::make_shared<BinaryOperatorExpression>(std::move(lhs), std::move(rhs), op_kind);
    }

    ~BinaryOperatorExpression() override = default;

    [[nodiscard]] NodeKind kind() const noexcept override {
        return NodeKind::BinaryOperatorExpression;
    }

    [[nodiscard]] bool compile_time_capable() const noexcept override {
        return _lhs->compile_time_capable() && _rhs->compile_time_capable();
    }

    [[nodiscard]] constexpr const ExpressionPtr& lhs() const noexcept { return _lhs; }
    [[nodiscard]] constexpr const ExpressionPtr& rhs() const noexcept { return _rhs; }
    [[nodiscard]] constexpr BinaryOperatorKind   op_kind() const noexcept { return _op_kind; }
};

}  // namespace cirrus::ast
