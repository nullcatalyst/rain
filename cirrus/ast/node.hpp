#pragma once

#include "cirrus/lang/location.hpp"
#include "cirrus/util/result.hpp"
#include "cirrus/util/retain.hpp"

namespace cirrus::ast {

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
    FloatExpression,
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

class Node : public std::enable_shared_from_this<Node> {
  protected:
    lang::Location _location;

  public:
    virtual ~Node() = default;

    [[nodiscard]] virtual constexpr NodeKind kind() const = 0;

    [[nodiscard]] lang::Location location() const noexcept { return _location; }
};

using NodePtr = std::shared_ptr<Node>;

}  // namespace cirrus::ast
