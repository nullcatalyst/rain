#pragma once

#include "rain/lang/ast/expr/expression.hpp"
#include "rain/lang/ast/type/struct.hpp"

namespace rain::lang::ast {

class ArrayLiteralExpression : public Expression {
    util::MaybeOwnedPtr<Type>                _type;
    std::vector<std::unique_ptr<Expression>> _elements;

    lex::Location _location;

  public:
    ArrayLiteralExpression(util::MaybeOwnedPtr<Type>                type,
                           std::vector<std::unique_ptr<Expression>> elements,
                           lex::Location                            location)
        : _type(std::move(type)), _elements(std::move(elements)), _location(location) {}
    ~ArrayLiteralExpression() override = default;

    // Expression
    [[nodiscard]] constexpr serial::ExpressionKind kind() const noexcept override {
        return serial::ExpressionKind::ArrayLiteral;
    }
    [[nodiscard]] constexpr absl::Nullable<Type*> type() const noexcept override {
        return _type.get();
    }
    [[nodiscard]] constexpr lex::Location location() const noexcept override { return _location; }
    [[nodiscard]] bool                    compile_time_capable() const noexcept override {
        for (const auto& element : _elements) {
            if (!element->compile_time_capable()) {
                return false;
            }
        }

        return true;
    }

    // StructLiteralExpression
    [[nodiscard]] constexpr const std::vector<std::unique_ptr<Expression>>& elements() const {
        return _elements;
    }
    [[nodiscard]] constexpr std::vector<std::unique_ptr<Expression>>& elements() {
        return _elements;
    }

    util::Result<void> validate(Options& options, Scope& scope) override;
};

}  // namespace rain::lang::ast
