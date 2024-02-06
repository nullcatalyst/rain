#pragma once

#include <string_view>

#include "cirrus/ast/expr/expression.hpp"

namespace cirrus::ast {

struct IdentifierExpressionData : public ExpressionData {
    std::string_view name;
};

DECLARE_EXPRESSION(Identifier) {
    EXPRESSION_COMMON_IMPL(Identifier);

    [[nodiscard]] static IdentifierExpression alloc(const std::string_view name) noexcept;

    [[nodiscard]] constexpr std::string_view name() const noexcept { return _data->name; }
};

}  // namespace cirrus::ast
