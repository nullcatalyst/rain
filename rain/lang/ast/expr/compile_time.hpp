#pragma once

#include <memory>

#include "rain/lang/ast/expr/expression.hpp"
#include "rain/lang/serial/kind.hpp"

namespace rain::lang::ast {

class CompileTimeExpression : public Expression {
    std::unique_ptr<ast::Expression> _expression;

  public:
    CompileTimeExpression(std::unique_ptr<Expression> expression)
        : _expression(std::move(expression)) {}

    [[nodiscard]] constexpr serial::ExpressionKind kind() const noexcept override {
        return serial::ExpressionKind::CompileTime;
    }
    [[nodiscard]] constexpr absl::Nullable<Type*> type() const noexcept override {
        return _expression->type();
    }
    [[nodiscard]] constexpr const ast::Expression& expression() const { return *_expression; }
    [[nodiscard]] constexpr ast::Expression&       expression() { return *_expression; }

    [[nodiscard]] bool compile_time_capable() const noexcept override {
        return _expression->compile_time_capable();
    }
    util::Result<void> validate(Scope& scope) override;
};

}  // namespace rain::lang::ast
