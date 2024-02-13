#pragma once

#include "cirrus/ast/expr/expression.hpp"
#include "cirrus/util/twine.hpp"

namespace cirrus::ast {

struct IdentifierExpressionData : public ExpressionData {
    util::String name;
};

DECLARE_EXPRESSION(Identifier) {
    EXPRESSION_COMMON_IMPL(Identifier);

    [[nodiscard]] static IdentifierExpression alloc(const util::String name) noexcept;

    [[nodiscard]] constexpr const util::String& name() const noexcept { return _data->name; }
};

}  // namespace cirrus::ast
