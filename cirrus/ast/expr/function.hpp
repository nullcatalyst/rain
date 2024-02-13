#pragma once

#include <vector>

#include "cirrus/ast/expr/expression.hpp"
#include "cirrus/ast/type/type.hpp"
#include "cirrus/util/twine.hpp"

namespace cirrus::ast {

struct FunctionArgumentData {
    util::String name;
    Type         type;
};

struct FunctionExpressionData : public ExpressionData {
    std::optional<util::String>       name;
    std::optional<Type>               return_type;
    std::vector<FunctionArgumentData> arguments;
    std::vector<Expression>           expressions;
};

DECLARE_EXPRESSION(Function) {
    EXPRESSION_COMMON_IMPL(Function);

    [[nodiscard]] static FunctionExpression alloc(
        std::optional<util::String> name, std::optional<Type> return_type,
        std::vector<FunctionArgumentData> arguments, std::vector<Expression> expressions) noexcept;

    [[nodiscard]] constexpr bool is_named() const noexcept { return _data->name != std::nullopt; }
    [[nodiscard]] constexpr const std::optional<util::String>& name() const noexcept {
        return _data->name;
    }
    [[nodiscard]] util::String name_or_empty() const noexcept {
        return _data->name.value_or(util::String{});
    }
    [[nodiscard]] constexpr bool has_return_type() const noexcept {
        return _data->return_type != std::nullopt;
    }
    [[nodiscard]] constexpr const std::optional<Type>& return_type() const noexcept {
        return _data->return_type;
    }
    [[nodiscard]] constexpr bool has_arguments() const noexcept {
        return !_data->arguments.empty();
    }
    [[nodiscard]] constexpr const std::vector<FunctionArgumentData>& arguments() const noexcept {
        return _data->arguments;
    }
    [[nodiscard]] constexpr const std::vector<Expression>& expressions() const noexcept {
        return _data->expressions;
    }
    [[nodiscard]] constexpr std::vector<Expression>& expressions() noexcept {
        return _data->expressions;
    }
};

}  // namespace cirrus::ast
