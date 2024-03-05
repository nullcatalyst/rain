#pragma once

#include <vector>

#include "rain/ast/expr/expression.hpp"

namespace rain::ast {

class CallExpression : public Expression {
    ExpressionPtr              _callee;
    std::vector<ExpressionPtr> _arguments;

    lang::Location _argument_list_location;  // From the opening '(' to the closing ')'

  public:
    CallExpression(ExpressionPtr callee, std::vector<ExpressionPtr> arguments)
        : _callee(std::move(callee)), _arguments(std::move(arguments)) {}
    CallExpression(ExpressionPtr callee, std::vector<ExpressionPtr> arguments,
                   const lang::Location argument_list_location)
        : Expression(callee->location().merge(argument_list_location)),
          _callee(std::move(callee)),
          _arguments(std::move(arguments)) {}

    [[nodiscard]] static std::unique_ptr<CallExpression> alloc(
        ExpressionPtr callee, std::vector<ExpressionPtr> arguments) noexcept {
        return std::make_unique<CallExpression>(std::move(callee), std::move(arguments));
    }
    [[nodiscard]] static std::unique_ptr<CallExpression> alloc(
        ExpressionPtr callee, std::vector<ExpressionPtr> arguments,
        const lang::Location argument_list_location) noexcept {
        return std::make_unique<CallExpression>(std::move(callee), std::move(arguments),
                                                argument_list_location);
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

}  // namespace rain::ast
