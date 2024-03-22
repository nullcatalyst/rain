#pragma once

#include <memory>

#include "rain/lang/ast/expr/expression.hpp"
#include "rain/lang/serial/kind.hpp"

namespace rain::lang::ast {

class ExportExpression : public Expression {
    std::unique_ptr<ast::Expression> _expression;

  public:
    ExportExpression(std::unique_ptr<Expression> expression) : _expression(std::move(expression)) {}

    [[nodiscard]] constexpr serial::ExpressionKind kind() const noexcept override {
        return serial::ExpressionKind::Export;
    }
    [[nodiscard]] constexpr absl::Nullable<Type*> type() const noexcept override {
        return _expression->type();
    }
    [[nodiscard]] constexpr const ast::Expression& expression() const { return *_expression; }
    [[nodiscard]] constexpr ast::Expression&       expression() { return *_expression; }

    util::Result<void> validate(Scope& scope) override;
};

}  // namespace rain::lang::ast
