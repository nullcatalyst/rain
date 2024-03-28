#pragma once

#include <memory>

#include "rain/lang/ast/expr/expression.hpp"
#include "rain/lang/serial/kind.hpp"

namespace rain::lang::ast {

class ExportExpression : public Expression {
    std::unique_ptr<ast::Expression> _expression;

    lex::Location _location;

  public:
    ExportExpression(std::unique_ptr<Expression> expression, lex::Location location)
        : _expression(std::move(expression)), _location(location) {}

    // Expression
    [[nodiscard]] constexpr serial::ExpressionKind kind() const noexcept override {
        return serial::ExpressionKind::Export;
    }
    [[nodiscard]] constexpr absl::Nullable<Type*> type() const noexcept override {
        return _expression->type();
    }
    [[nodiscard]] constexpr lex::Location location() const noexcept override { return _location; }

    // ExportExpression
    [[nodiscard]] constexpr const ast::Expression& expression() const { return *_expression; }
    [[nodiscard]] constexpr ast::Expression&       expression() { return *_expression; }

    util::Result<void> validate(Scope& scope) override;
};

}  // namespace rain::lang::ast
