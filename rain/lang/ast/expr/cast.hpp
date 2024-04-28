#pragma once

#include <memory>

#include "absl/base/nullability.h"
#include "rain/lang/ast/expr/expression.hpp"
#include "rain/lang/serial/kind.hpp"

namespace rain::lang::ast {

class CastExpression : public Expression {
    std::unique_ptr<ast::Expression>  _expression;
    absl::Nullable<FunctionVariable*> _method = nullptr;
    absl::Nullable<Type*>             _type   = nullptr;

    lex::Location _op_location;
    lex::Location _type_location;

  public:
    CastExpression(std::unique_ptr<Expression> expression, absl::Nullable<Type*> type,
                   lex::Location op_location, lex::Location type_location)
        : _expression(std::move(expression)),
          _type(type),
          _op_location(op_location),
          _type_location(type_location) {}

    // Expression
    [[nodiscard]] constexpr serial::ExpressionKind kind() const noexcept override {
        return serial::ExpressionKind::Cast;
    }
    [[nodiscard]] constexpr absl::Nullable<Type*> type() const noexcept override { return _type; }
    [[nodiscard]] constexpr lex::Location         location() const noexcept override {
        return _expression->location().merge(_type_location);
    }

    [[nodiscard]] bool is_compile_time_capable() const noexcept override;

    // CastExpression
    [[nodiscard]] /*constexpr*/ const ast::Expression&  expression() const { return *_expression; }
    [[nodiscard]] /*constexpr*/ ast::Expression&        expression() { return *_expression; }
    [[nodiscard]] /*constexpr*/ const FunctionVariable* method() const { return _method; }
    [[nodiscard]] /*constexpr*/ FunctionVariable*       method() { return _method; }

    util::Result<void> validate(Options& options, Scope& scope) override;
};

}  // namespace rain::lang::ast
