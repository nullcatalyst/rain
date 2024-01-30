#pragma once

#include "cirrus/ast/node.hpp"

namespace cirrus::ast {

struct ExpressionVtbl : public NodeVtbl {};

struct ExpressionData : public NodeData {};

template <typename This, typename Vtbl, typename Data>
struct IExpression : public INode<This, Vtbl, Data> {
    using INode<This, Vtbl, Data>::INode;
};

struct Expression : public IExpression<Expression, ExpressionVtbl, ExpressionData> {
    using IExpression<Expression, ExpressionVtbl, ExpressionData>::IExpression;

    [[nodiscard]] constexpr lang::Location location() const noexcept { return _data->location; }
};

}  // namespace cirrus::ast
