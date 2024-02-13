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
    FunctionType,
    InterfaceType,
    OpaqueType,
    StructType,
    UnresolvedType,
    // </keep-sorted>

    // Expressions
    // <keep-sorted>
    BinaryOperatorExpression,
    BlockExpression,
    CallExpression,
    ExecExpression,
    ExportExpression,
    FunctionExpression,
    IdentifierExpression,
    IfExpression,
    IntegerExpression,
    LetExpression,
    MemberExpression,
    ParenthesisExpression,
    ReturnExpression,
    UnaryOperatorExpression,
    // </keep-sorted>

    Count,
};

struct NodeVtbl : public util::RetainableVtbl {
    NodeKind _kind;
};

struct NodeData : public util::RetainableData {
    lang::Location _location;
};

template <typename This, typename Vtbl, typename Data>
struct INode : public util::Retainable<This, Vtbl, Data> {
    using util::Retainable<This, Vtbl, Data>::Retainable;

    [[nodiscard]] constexpr NodeKind kind() const { return this->_vtbl->_kind; }
    [[nodiscard]] lang::Location     location() const noexcept { return this->_data->_location; }
};

struct Node : public INode<Node, NodeVtbl, NodeData> {
    using INode<Node, NodeVtbl, NodeData>::INode;
};

}  // namespace cirrus::ast
