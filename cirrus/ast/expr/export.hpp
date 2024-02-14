#pragma once

#include <vector>

#include "cirrus/ast/expr/expression.hpp"

namespace cirrus::ast {

class ExportExpression : public Expression {
    ExpressionPtr _expression;

  public:
    ExportExpression(ExpressionPtr expression) : _expression(std::move(expression)) {}

    [[nodiscard]] static std::shared_ptr<ExportExpression> alloc(ExpressionPtr expression) {
        return std::make_shared<ExportExpression>(std::move(expression));
    }

    [[nodiscard]] NodeKind kind() const noexcept override { return NodeKind::ExportExpression; }

    [[nodiscard]] bool compile_time_capable() const noexcept override { return false; }

    [[nodiscard]] const ExpressionPtr& expression() const noexcept { return _expression; }
};

}  // namespace cirrus::ast
