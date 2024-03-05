#pragma once

#include "rain/ast/expr/expression.hpp"
#include "rain/lang/location.hpp"

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

    lang::Location _op_location;

  public:
    BinaryOperatorExpression(ExpressionPtr lhs, ExpressionPtr rhs,
                             BinaryOperatorKind op_kind) noexcept
        : Expression(lhs->location().merge(rhs->location())),
          _lhs(std::move(lhs)),
          _rhs(std::move(rhs)),
          _op_kind(op_kind) {}
    BinaryOperatorExpression(ExpressionPtr lhs, ExpressionPtr rhs, BinaryOperatorKind op_kind,
                             const lang::Location op_location) noexcept
        : Expression(lhs->location().merge(rhs->location())),
          _lhs(std::move(lhs)),
          _rhs(std::move(rhs)),
          _op_kind(op_kind),
          _op_location(op_location) {}

    [[nodiscard]] static std::unique_ptr<BinaryOperatorExpression> alloc(
        ExpressionPtr lhs, ExpressionPtr rhs, BinaryOperatorKind op_kind) {
        return std::make_unique<BinaryOperatorExpression>(std::move(lhs), std::move(rhs), op_kind);
    }
    [[nodiscard]] static std::unique_ptr<BinaryOperatorExpression> alloc(
        ExpressionPtr lhs, ExpressionPtr rhs, BinaryOperatorKind op_kind,
        const lang::Location op_location) {
        return std::make_unique<BinaryOperatorExpression>(std::move(lhs), std::move(rhs), op_kind,
                                                          op_location);
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
    [[nodiscard]] constexpr const lang::Location op_location() const noexcept {
        return _op_location;
    }
};

}  // namespace rain::ast
