#pragma once

#include <memory>

#include "rain/lang/ast/expr/expression.hpp"
#include "rain/lang/serial/kind.hpp"

namespace rain::lang::ast {

class CompileTimeExpression : public Expression {
    std::unique_ptr<ast::Expression> _expression;
    lex::Location                    _location;

  public:
    CompileTimeExpression(std::unique_ptr<Expression> expression, lex::Location location)
        : _expression(std::move(expression)), _location(location) {}

    // Expression
    [[nodiscard]] constexpr serial::ExpressionKind kind() const noexcept override {
        return serial::ExpressionKind::CompileTime;
    }
    [[nodiscard]] constexpr absl::Nullable<Type*> type() const noexcept override {
        return _expression->type();
    }
    [[nodiscard]] constexpr lex::Location location() const noexcept override { return _location; }

    // CompileTimeExpression
    [[nodiscard]] constexpr const ast::Expression& expression() const { return *_expression; }
    [[nodiscard]] constexpr ast::Expression&       expression() { return *_expression; }

    [[nodiscard]] bool compile_time_capable() const noexcept override {
        if (_expression->kind() == serial::ExpressionKind::CompileTime) {
            // There is no need to try to evaluate nested compile-time expressions at compile-time,
            // as the innermost one will be evaluated at compile-time, all the outer ones will be
            // too automatically.
            return false;
        }
        return _expression->compile_time_capable();
    }
    util::Result<void> validate(Scope& scope) override;
};

}  // namespace rain::lang::ast
