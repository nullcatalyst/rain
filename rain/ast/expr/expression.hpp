#pragma once

#include <memory>

#include "rain/ast/type/type.hpp"
#include "rain/lang/location.hpp"

namespace rain::ast {

/**
 * A list of all possible AST node kinds.
 *
 * This enum is used to identify the kind of a node without having to use RTTI, virtual functions,
 * dynamic casts, or the visitor pattern. This should be the fastest way to identify a node kind.
 */
enum class ExpressionKind {
    Unknown,

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
    TypeDeclarationExpression,
    UnaryOperatorExpression,
    // </keep-sorted>

    Count,
};

class Expression {
  protected:
    ExpressionKind _kind;
    lang::Location _location;
    TypePtr        _type;

  public:
    virtual ~Expression() = default;

    [[nodiscard]] virtual constexpr ExpressionKind kind() const noexcept = 0;
    [[nodiscard]] constexpr const lang::Location&  location() const noexcept { return _location; }
    [[nodiscard]] constexpr const TypePtr&         type() const noexcept { return _type; }
    // void set_type(TypePtr type) noexcept { _type = std::move(type); }

    [[nodiscard]] virtual bool compile_time_capable() const noexcept = 0;
};

using ExpressionPtr = std::unique_ptr<Expression>;

}  // namespace rain::ast
