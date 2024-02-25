#pragma once

#include "cirrus/ast/expr/expression.hpp"
#include "cirrus/util/string.hpp"

namespace cirrus::ast {

class IdentifierExpression : public Expression {
    util::String _name;

  public:
    IdentifierExpression(util::String name) : _name(std::move(name)) {}

    [[nodiscard]] static std::unique_ptr<IdentifierExpression> alloc(util::String name) {
        return std::make_unique<IdentifierExpression>(std::move(name));
    }

    [[nodiscard]] constexpr ExpressionKind kind() const noexcept override {
        return ExpressionKind::IdentifierExpression;
    }

    [[nodiscard]] bool compile_time_capable() const noexcept override {
        // TODO: This must be determined based on the variable's declaration
        //   1. If the variable is not declared as mutable, using a compile-time-capable value, then
        //      this should return true. (trivial case)
        //   2. If the variable is declared as mutable, BUT assigned using a non-compile-time-
        //      capable value AND there is no mutation in between the declaration and the usage,
        //      then this should return true.
        //   3. If the variable is declared as mutable AND (assigned using a non-compile-time-
        //      capable value OR there is mutation in between the declaration and the usage), then
        //      this should return false.
        return true;
    }

    [[nodiscard]] util::String name() const noexcept { return _name; }
};

}  // namespace cirrus::ast
