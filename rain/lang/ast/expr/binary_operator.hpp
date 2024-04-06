#pragma once

#include <memory>

#include "rain/lang/ast/expr/expression.hpp"
#include "rain/lang/serial/kind.hpp"

namespace rain::lang::ast {

class BinaryOperatorExpression : public Expression {
    std::unique_ptr<ast::Expression> _lhs;
    std::unique_ptr<ast::Expression> _rhs;
    serial::BinaryOperatorKind       _op;

    absl::Nullable<FunctionVariable*> _method = nullptr;
    absl::Nullable<Type*>             _type   = nullptr;

    lex::Location _op_location;

  public:
    BinaryOperatorExpression(std::unique_ptr<Expression> lhs, std::unique_ptr<Expression> rhs,
                             serial::BinaryOperatorKind op)
        : _lhs(std::move(lhs)), _rhs(std::move(rhs)), _op(op) {}
    BinaryOperatorExpression(std::unique_ptr<Expression> lhs, std::unique_ptr<Expression> rhs,
                             serial::BinaryOperatorKind op, lex::Location op_location)
        : _lhs(std::move(lhs)), _rhs(std::move(rhs)), _op(op), _op_location(op_location) {}

    // Expression
    [[nodiscard]] constexpr serial::ExpressionKind kind() const noexcept override {
        return serial::ExpressionKind::BinaryOperator;
    }
    [[nodiscard]] constexpr absl::Nullable<Type*> type() const noexcept override { return _type; }
    [[nodiscard]] /*constexpr*/ lex::Location     location() const noexcept override {
        return _lhs->location().merge(_rhs->location());
    }

    // BinaryOperatorExpression
    [[nodiscard]] /*constexpr*/ const ast::Expression&        lhs() const { return *_lhs; }
    [[nodiscard]] /*constexpr*/ ast::Expression&              lhs() { return *_lhs; }
    [[nodiscard]] /*constexpr*/ const ast::Expression&        rhs() const { return *_rhs; }
    [[nodiscard]] /*constexpr*/ ast::Expression&              rhs() { return *_rhs; }
    [[nodiscard]] constexpr serial::BinaryOperatorKind        op() const { return _op; }
    [[nodiscard]] constexpr absl::Nullable<FunctionVariable*> method() const noexcept {
        return _method;
    }
    [[nodiscard]] constexpr lex::Location op_location() const noexcept { return _op_location; }

    [[nodiscard]] bool compile_time_capable() const noexcept override;
    util::Result<void> validate(Options& options, Scope& scope) override;
};

}  // namespace rain::lang::ast
