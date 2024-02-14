#pragma once

#include <vector>

#include "cirrus/ast/expr/expression.hpp"

namespace cirrus::ast {

class CallExpression : public Expression {
    ExpressionPtr              _callee;
    std::vector<ExpressionPtr> _arguments;

  public:
    CallExpression(ExpressionPtr callee, std::vector<ExpressionPtr> arguments)
        : _callee(std::move(callee)), _arguments(std::move(arguments)) {}

    [[nodiscard]] static std::shared_ptr<CallExpression> alloc(
        ExpressionPtr callee, std::vector<ExpressionPtr> arguments) noexcept {
        return std::make_shared<CallExpression>(std::move(callee), std::move(arguments));
    }

    [[nodiscard]] NodeKind kind() const noexcept override { return NodeKind::CallExpression; }

    [[nodiscard]] bool compile_time_capable() const noexcept override;

    [[nodiscard]] const ExpressionPtr&              callee() const noexcept { return _callee; }
    [[nodiscard]] const std::vector<ExpressionPtr>& arguments() const& noexcept {
        return _arguments;
    }
    [[nodiscard]] std::vector<ExpressionPtr>&& arguments() && noexcept {
        return std::move(_arguments);
    }
};

}  // namespace cirrus::ast
