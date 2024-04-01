#pragma once

#include <memory>

#include "rain/lang/ast/expr/expression.hpp"
#include "rain/lang/serial/kind.hpp"

namespace rain::lang::ast {

class UnaryOperatorExpression : public Expression {
    std::unique_ptr<ast::Expression> _expression;
    serial::UnaryOperatorKind        _op;

    absl::Nullable<FunctionVariable*> _method = nullptr;
    absl::Nullable<Type*>             _type   = nullptr;

    lex::Location _op_location;

  public:
    UnaryOperatorExpression(std::unique_ptr<Expression> expression, serial::UnaryOperatorKind op,
                            lex::Location op_location)
        : _expression(std::move(expression)), _op(op), _op_location(op_location) {}

    // Expression
    [[nodiscard]] constexpr serial::ExpressionKind kind() const noexcept override {
        return serial::ExpressionKind::UnaryOperator;
    }
    [[nodiscard]] constexpr absl::Nullable<Type*> type() const noexcept override { return _type; }
    [[nodiscard]] constexpr lex::Location         location() const noexcept override {
        return _op_location.merge(_expression->location());
    }

    // UnaryOperatorExpression
    [[nodiscard]] constexpr const ast::Expression&    expression() const { return *_expression; }
    [[nodiscard]] constexpr ast::Expression&          expression() { return *_expression; }
    [[nodiscard]] constexpr serial::UnaryOperatorKind op() const { return _op; }
    [[nodiscard]] constexpr absl::Nullable<FunctionVariable*> method() const noexcept {
        return _method;
    }

    util::Result<void> validate(Options& options, Scope& scope) override;
};

}  // namespace rain::lang::ast
