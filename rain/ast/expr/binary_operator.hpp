#pragma once

#include "rain/ast/expr/expression.hpp"

namespace rain::ast {

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

    [[nodiscard]] static std::unique_ptr<BinaryOperatorExpression> alloc(
        ExpressionPtr lhs, ExpressionPtr rhs, BinaryOperatorKind op_kind) {
        return std::make_unique<BinaryOperatorExpression>(std::move(lhs), std::move(rhs), op_kind);
    }

    ~BinaryOperatorExpression() override = default;

    [[nodiscard]] constexpr ExpressionKind kind() const noexcept override {
        return ExpressionKind::BinaryOperatorExpression;
    }

    [[nodiscard]] bool compile_time_capable() const noexcept override {
        return _lhs->compile_time_capable() && _rhs->compile_time_capable();
    }

    [[nodiscard]] constexpr const ExpressionPtr& lhs() const noexcept { return _lhs; }
    [[nodiscard]] constexpr const ExpressionPtr& rhs() const noexcept { return _rhs; }
    [[nodiscard]] constexpr BinaryOperatorKind   op_kind() const noexcept { return _op_kind; }
};

}  // namespace rain::ast
