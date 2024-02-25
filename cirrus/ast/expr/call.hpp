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

    [[nodiscard]] static std::unique_ptr<CallExpression> alloc(
        ExpressionPtr callee, std::vector<ExpressionPtr> arguments) noexcept {
        return std::make_unique<CallExpression>(std::move(callee), std::move(arguments));
    }

    [[nodiscard]] constexpr ExpressionKind kind() const noexcept override {
        return ExpressionKind::CallExpression;
    }

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
