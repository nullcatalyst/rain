#pragma once

#include "cirrus/ast/expr/all.hpp"
#include "cirrus/ast/type/all.hpp"

namespace cirrus::transform {

class Visiter {
  public:
    Visiter()          = default;
    virtual ~Visiter() = default;

    // Types
    virtual void visit(ast::StructType& type);
    virtual void visit(ast::UnresolvedType& type);

    // Expressions
    virtual void visit(ast::IntegerExpression& expr);
    virtual void visit(ast::IdentifierExpression& expr);
    virtual void visit(ast::ParenthesisExpression& expr);
    // virtual void visit(ast::UnaryOperatorExpression& expr);
    virtual void visit(ast::BinaryOperatorExpression& expr);
    virtual void visit(ast::CallExpression& expr);
};

}  // namespace cirrus::transform
