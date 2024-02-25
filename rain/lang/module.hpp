#pragma once

#include <vector>

#include "rain/ast/expr/expression.hpp"

namespace rain::lang {

class Validator;
class Compiler;

class Module {
    std::vector<ast::ExpressionPtr> _expressions;

  public:
    Module() = default;
    Module(std::vector<ast::ExpressionPtr> expressions) : _expressions(std::move(expressions)) {}

    [[nodiscard]] constexpr const std::vector<ast::ExpressionPtr>& expressions() const& noexcept {
        return _expressions;
    }
    [[nodiscard]] constexpr std::vector<ast::ExpressionPtr>&& expressions() && noexcept {
        return std::move(_expressions);
    }
};

// Use compile-time type information to ensure that only valid modules are compilable

class ParsedModule : public Module {
    friend class ::rain::lang::Validator;

  public:
    using Module::Module;
};

class ValidatedModule : public Module {
    friend class ::rain::lang::Compiler;

  public:
    using Module::Module;
};

}  // namespace rain::lang
