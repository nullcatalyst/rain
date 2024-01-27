#pragma once

#include <string_view>

#include "cirrus/ast/node.hpp"

namespace cirrus::ast {

struct TypeVtbl : public NodeVtbl {};

struct TypeData : public NodeData {};

template <typename This, typename Vtbl, typename Data>
struct IType : public INode<This, Vtbl, Data> {
    using INode<This, Vtbl, Data>::INode;
};

struct Type : public IType<Type, TypeVtbl, TypeData> {
    using IType<Type, TypeVtbl, TypeData>::IType;
};

}  // namespace cirrus::ast
