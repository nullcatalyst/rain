#pragma once

#include <vector>

#include "cirrus/ast/expr/expression.hpp"

namespace cirrus::ast {

struct CallExpressionData : public ExpressionData {
    Expression              callee;
    std::vector<Expression> arguments;
};

struct CallExpression : public IExpression<CallExpression, ExpressionVtbl, CallExpressionData> {
    using IExpression<CallExpression, ExpressionVtbl, CallExpressionData>::IExpression;

    static const ExpressionVtbl _vtbl;

    [[nodiscard]] static bool is(const Expression expr) noexcept { return expr.vtbl() == &_vtbl; }
    [[nodiscard]] static CallExpression alloc(Expression              callee,
                                              std::vector<Expression> expressions) noexcept;

    [[nodiscard]] constexpr const Expression& callee() const noexcept { return _data->callee; }
    [[nodiscard]] constexpr const std::vector<Expression>& arguments() const noexcept {
        return _data->arguments;
    }
};

}  // namespace cirrus::ast
