#pragma once

#include "cirrus/ast/expr/expression.hpp"
#include "cirrus/ast/type/type.hpp"

namespace cirrus::ast {

struct FunctionArgumentData {
    std::string_view name;
    Type             type;
};

struct FunctionExpressionData : public ExpressionData {
    std::optional<std::string_view>   name;
    std::optional<Type>               return_type;
    std::vector<FunctionArgumentData> arguments;
    std::vector<Expression>           expressions;
};

struct FunctionExpression
    : public IExpression<FunctionExpression, ExpressionVtbl, FunctionExpressionData> {
    using IExpression<FunctionExpression, ExpressionVtbl, FunctionExpressionData>::IExpression;

    static const ExpressionVtbl _vtbl;

    [[nodiscard]] static bool is(const Expression expr) noexcept { return expr.vtbl() == &_vtbl; }
    [[nodiscard]] static FunctionExpression alloc(std::optional<std::string_view>   name,
                                                  std::optional<Type>               return_type,
                                                  std::vector<FunctionArgumentData> arguments,
                                                  std::vector<Expression> expressions) noexcept;

    [[nodiscard]] constexpr bool is_named() const noexcept { return _data->name != std::nullopt; }
    [[nodiscard]] constexpr std::optional<std::string_view> name() const noexcept {
        return _data->name;
    }
    [[nodiscard]] constexpr std::string_view name_or_empty() const noexcept {
        return _data->name.value_or("");
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
