#pragma once

#include <vector>

#include "cirrus/ast/expr/expression.hpp"

namespace cirrus::ast {

struct LetExpressionData : public ExpressionData {
    std::string_view _name;
    Expression       _value;
    bool             _mutable;
};

DECLARE_EXPRESSION(Let) {
    EXPRESSION_COMMON_IMPL(Let);

    [[nodiscard]] static LetExpression alloc(std::string_view name, Expression expression,
                                             bool mutable_) noexcept;

    [[nodiscard]] constexpr std::string_view  name() const noexcept { return _data->_name; }
    [[nodiscard]] constexpr const Expression& value() const noexcept { return _data->_value; }
    [[nodiscard]] constexpr bool              mutable_() const noexcept { return _data->_mutable; }
};

}  // namespace cirrus::ast
