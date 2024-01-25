#pragma once

#include "cirrus/util/retain.hpp"

namespace cirrus::ast {

struct ExpressionVtbl : public util::RetainableVtbl {};

struct ExpressionData : public util::RetainableData {};

template <typename This, typename Vtbl, typename Data>
struct IExpression : public util::Retainable<This, Vtbl, Data> {
    using util::Retainable<This, Vtbl, Data>::Retainable;
};

struct Expression : public IExpression<Expression, ExpressionVtbl, ExpressionData> {
    using IExpression<Expression, ExpressionVtbl, ExpressionData>::IExpression;
};

}  // namespace cirrus::ast
