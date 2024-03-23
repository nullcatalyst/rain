#pragma once

#include <vector>

#include "absl/base/nullability.h"
#include "rain/lang/ast/expr/expression.hpp"
#include "rain/lang/ast/scope/module.hpp"
#include "rain/lang/ast/type/function.hpp"

namespace rain::lang::ast {

class Module {
    std::vector<std::unique_ptr<Expression>> _expressions;
    ast::ModuleScope                         _scope;

  public:
    explicit Module(ast::BuiltinScope& builtin) : _scope(builtin) {}
    ~Module() = default;

    [[nodiscard]] constexpr serial::ExpressionKind kind() const noexcept {
        return serial::ExpressionKind::Unknown;
    }
    [[nodiscard]] constexpr const std::vector<std::unique_ptr<Expression>>& expressions()
        const noexcept {
        return _expressions;
    }
    [[nodiscard]] constexpr ast::ModuleScope& scope() noexcept { return _scope; }

    void add_expression(std::unique_ptr<Expression> expression) {
        _expressions.push_back(std::move(expression));
    }

    util::Result<void> validate();
};

}  // namespace rain::lang::ast
