#pragma once

#include <vector>

#include "cirrus/ast/expr/expression.hpp"
#include "cirrus/ast/type/type.hpp"
#include "cirrus/util/string.hpp"

namespace cirrus::ast {

struct FunctionArgumentData {
    util::String name;
    TypePtr      type;
};

class FunctionExpression : public Expression {
    std::optional<util::String>       _name;
    std::optional<TypePtr>            _return_type;
    std::vector<FunctionArgumentData> _arguments;
    std::vector<ExpressionPtr>        _expressions;

  public:
    FunctionExpression(std::optional<util::String> name, std::optional<TypePtr> return_type,
                       std::vector<FunctionArgumentData> arguments,
                       std::vector<ExpressionPtr>        expressions)
        : _name{std::move(name)},
          _return_type{std::move(return_type)},
          _arguments{std::move(arguments)},
          _expressions{std::move(expressions)} {}

    [[nodiscard]] static std::unique_ptr<FunctionExpression> alloc(
        std::optional<util::String> name, std::optional<TypePtr> return_type,
        std::vector<FunctionArgumentData> arguments, std::vector<ExpressionPtr> expressions) {
        return std::make_unique<FunctionExpression>(std::move(name), std::move(return_type),
                                                    std::move(arguments), std::move(expressions));
    }

    [[nodiscard]] constexpr ExpressionKind kind() const noexcept override {
        return ExpressionKind::FunctionExpression;
    }

    [[nodiscard]] bool compile_time_capable() const noexcept override {
        // TODO: Should this always return true?
        // Sure, we can always compile the function, but the function could access global mutable
        // variables. We should probably check for that.
        return true;
    }

    [[nodiscard]] constexpr bool is_named() const noexcept { return _name.has_value(); }
    [[nodiscard]] util::String   name_or_empty() const noexcept {
        return _name.value_or(util::String{});
    }
    [[nodiscard]] constexpr const std::optional<TypePtr>& return_type() const noexcept {
        return _return_type;
    }
    [[nodiscard]] constexpr bool has_arguments() const noexcept { return !_arguments.empty(); }
    [[nodiscard]] constexpr const std::vector<FunctionArgumentData>& arguments() const& noexcept {
        return _arguments;
    }
    [[nodiscard]] constexpr std::vector<FunctionArgumentData>&& arguments() && noexcept {
        return std::move(_arguments);
    }
    [[nodiscard]] constexpr const std::vector<ExpressionPtr>& expressions() const& noexcept {
        return _expressions;
    }
    [[nodiscard]] constexpr std::vector<ExpressionPtr>&& expressions() && noexcept {
        return std::move(_expressions);
    }
};

}  // namespace cirrus::ast
