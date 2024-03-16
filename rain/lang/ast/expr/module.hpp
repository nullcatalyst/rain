#pragma once

#include <vector>

#include "rain/lang/ast/expr/expression.hpp"
#include "rain/lang/ast/scope/module.hpp"

namespace rain::lang::ast {

class Module {
    std::vector<std::unique_ptr<Expression>> _expressions;
    ast::ModuleScope                         _scope;

  public:
    explicit Module(ast::BuiltinScope& builtin) : _scope(builtin) {}
    ~Module() = default;

    [[nodiscard]] constexpr const std::vector<std::unique_ptr<Expression>>& expressions()
        const noexcept {
        return _expressions;
    }

    [[nodiscard]] constexpr ast::ModuleScope& scope() noexcept { return _scope; }

    void add_expression(std::unique_ptr<Expression> expression) {
        _expressions.push_back(std::move(expression));
    }
};

}  // namespace rain::lang::ast