#pragma once

#include "rain/lang/ast/expr/expression.hpp"

namespace rain::lang::ast {

class TupleExpression : public Expression {
  public:
    TupleExpression()           = default;
    ~TupleExpression() override = default;

    // Expression
    [[nodiscard]] constexpr serial::ExpressionKind kind() const noexcept override {
        return serial::ExpressionKind::Tuple;
    }
    [[nodiscard]] constexpr absl::Nullable<Type*> type() const noexcept override {
        return _expression->type();
    }
    [[nodiscard]] constexpr lex::Location location() const noexcept override { return _location; }

    [[nodiscard]] bool is_compile_time_capable() const noexcept override {
        return _expression->is_compile_time_capable();
    }

    // ParenthesisExpression
    [[nodiscard]] constexpr const ast::Expression& expression() const { return *_expression; }
    [[nodiscard]] constexpr ast::Expression&       expression() { return *_expression; }

    util::Result<void> validate(Options& options, Scope& scope) override;
};

}  // namespace rain::lang::ast
