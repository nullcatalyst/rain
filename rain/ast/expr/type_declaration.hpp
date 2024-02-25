#pragma once

#include "rain/ast/expr/expression.hpp"

namespace rain::ast {

class TypeDeclarationExpression : public Expression {
    TypePtr _type;

  public:
    TypeDeclarationExpression(TypePtr type) : _type(std::move(type)) {}

    [[nodiscard]] static std::unique_ptr<TypeDeclarationExpression> alloc(TypePtr type) {
        return std::make_unique<TypeDeclarationExpression>(std::move(type));
    }

    [[nodiscard]] constexpr ExpressionKind kind() const noexcept override {
        return ExpressionKind::TypeDeclarationExpression;
    }
    [[nodiscard]] const TypePtr& type() const noexcept { return _type; }

    [[nodiscard]] bool compile_time_capable() const noexcept override { return false; }
};

}  // namespace rain::ast
