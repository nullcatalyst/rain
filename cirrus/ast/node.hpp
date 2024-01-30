#pragma once

#include "cirrus/lang/location.hpp"
#include "cirrus/util/result.hpp"
#include "cirrus/util/retain.hpp"

namespace cirrus::ast {

#define OK_ALLOC(ResultType, ...) OK(ResultType, ResultType::alloc(__VA_ARGS__))

/**
 * A list of all possible AST node kinds.
 *
 * This enum is used to identify the kind of a node without having to use RTTI, virtual functions,
 * dynamic casts, or the visitor pattern. This should be the fastest way to identify a node kind.
 */
enum class NodeKind {
    Unknown,

    // Types
    // <keep-sorted>
    StructType,
    UnresolvedType,
    // </keep-sorted>

    // Expressions
    // <keep-sorted>
    BinaryOperatorExpression,
    BlockExpression,
    CallExpression,
    FunctionExpression,
    IdentifierExpression,
    IfExpression,
    IntegerExpression,
    MemberExpression,
    ParenthesisExpression,
    ReturnExpression,
    UnaryOperatorExpression,
    // </keep-sorted>
};

struct NodeVtbl : public util::RetainableVtbl {
    NodeKind kind;
};

struct NodeData : public util::RetainableData {
    lang::Location location;
};

template <typename This, typename Vtbl, typename Data>
struct INode : public util::Retainable<This, Vtbl, Data> {
    using util::Retainable<This, Vtbl, Data>::Retainable;

    [[nodiscard]] constexpr NodeKind kind() const { return this->_vtbl->kind; }
};

struct Node : public INode<Node, NodeVtbl, NodeData> {
    using INode<Node, NodeVtbl, NodeData>::INode;

    [[nodiscard]] constexpr lang::Location location() const noexcept { return _data->location; }
};

}  // namespace cirrus::ast
