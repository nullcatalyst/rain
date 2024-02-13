#pragma once

#include "cirrus/ast/expr/expression.hpp"
#include "cirrus/util/twine.hpp"

namespace cirrus::ast {

struct LetExpressionData : public ExpressionData {
    util::Twine _name;
    Expression  _value;
    bool        _mutable;
};

DECLARE_EXPRESSION(Let) {
    EXPRESSION_COMMON_IMPL(Let);

    [[nodiscard]] static LetExpression alloc(util::Twine name, Expression expression,
                                             bool mutable_) noexcept;

    [[nodiscard]] constexpr const util::Twine& name() const noexcept { return _data->_name; }
    [[nodiscard]] constexpr const Expression&  value() const noexcept { return _data->_value; }
    [[nodiscard]] constexpr bool               mutable_() const noexcept { return _data->_mutable; }
};

}  // namespace cirrus::ast
