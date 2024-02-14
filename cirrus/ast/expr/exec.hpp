#pragma once

#include <vector>

#include "cirrus/ast/expr/expression.hpp"

namespace cirrus::ast {

class ExecExpression : public Expression {
    ExpressionPtr _expression;

  public:
    ExecExpression(ExpressionPtr expression) : _expression(std::move(expression)) {}

    [[nodiscard]] static std::shared_ptr<ExecExpression> alloc(ExpressionPtr expression) {
        return std::make_shared<ExecExpression>(std::move(expression));
    }

    [[nodiscard]] NodeKind kind() const noexcept override { return NodeKind::ExecExpression; }

    [[nodiscard]] bool compile_time_capable() const noexcept override {
        return _expression->compile_time_capable();
    }

    [[nodiscard]] const ExpressionPtr& expression() const noexcept { return _expression; }
};

}  // namespace cirrus::ast
