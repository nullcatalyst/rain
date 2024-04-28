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
        : _expression(std::move(expression)), _location(location) {
        assert(_expression != nullptr && "CompileTimeExpression cannot contain a nullptr");
        assert(_expression->kind() != serial::ExpressionKind::CompileTime &&
               "CompileTimeExpression cannot contain another CompileTimeExpression");
    }

    // Expression
    [[nodiscard]] constexpr serial::ExpressionKind kind() const noexcept override {
        return serial::ExpressionKind::CompileTime;
    }
    [[nodiscard]] constexpr absl::Nullable<Type*> type() const noexcept override {
        return _expression->type();
    }
    [[nodiscard]] constexpr lex::Location location() const noexcept override { return _location; }

    [[nodiscard]] bool is_compile_time_capable() const noexcept override {
        return _expression->is_compile_time_capable();
    }

    // CompileTimeExpression
    [[nodiscard]] /*constexpr*/ const ast::Expression& expression() const { return *_expression; }
    [[nodiscard]] /*constexpr*/ ast::Expression&       expression() { return *_expression; }

    util::Result<void> validate(Options& options, Scope& scope) override;
};

}  // namespace rain::lang::ast
