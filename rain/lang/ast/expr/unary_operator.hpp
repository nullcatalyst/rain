#pragma once

#include <memory>

#include "rain/lang/ast/expr/expression.hpp"
#include "rain/lang/serial/kind.hpp"

namespace rain::lang::ast {

class UnaryOperatorExpression : public Expression {
    std::unique_ptr<ast::Expression> _rhs;
    serial::UnaryOperatorKind        _op;

    absl::Nullable<FunctionVariable*> _method = nullptr;
    absl::Nullable<Type*>             _type   = nullptr;

  public:
    UnaryOperatorExpression(std::unique_ptr<Expression> rhs, serial::UnaryOperatorKind op)
        : _rhs(std::move(rhs)), _op(op) {}

    [[nodiscard]] constexpr serial::ExpressionKind kind() const noexcept override {
        return serial::ExpressionKind::UnaryOperator;
    }
    [[nodiscard]] constexpr absl::Nullable<Type*>  type() const noexcept override { return _type; }
    [[nodiscard]] constexpr const ast::Expression& rhs() const { return *_rhs; }
    [[nodiscard]] constexpr ast::Expression&       rhs() { return *_rhs; }
    [[nodiscard]] constexpr serial::UnaryOperatorKind         op() const { return _op; }
    [[nodiscard]] constexpr absl::Nullable<FunctionVariable*> method() const noexcept {
        return _method;
    }

    util::Result<void> validate(Scope& scope) override;
};

}  // namespace rain::lang::ast
