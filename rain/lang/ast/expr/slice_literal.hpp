#pragma once

#include "rain/lang/ast/expr/expression.hpp"
#include "rain/lang/ast/type/type.hpp"

namespace rain::lang::ast {

class SliceLiteralExpression : public Expression {
  public:
    static constexpr auto Kind = serial::ExpressionKind::SliceLiteral;

  private:
    absl::Nonnull<Type*>                     _type;
    std::vector<std::unique_ptr<Expression>> _elements;

    lex::Location _location;

  public:
    SliceLiteralExpression(absl::Nonnull<Type*>                     type,
                           std::vector<std::unique_ptr<Expression>> elements,
                           lex::Location                            location);
    ~SliceLiteralExpression() override = default;

    // Expression
    [[nodiscard]] constexpr serial::ExpressionKind kind() const noexcept override { return Kind; }
    [[nodiscard]] constexpr absl::Nullable<Type*>  type() const noexcept override { return _type; }
    [[nodiscard]] constexpr lex::Location location() const noexcept override { return _location; }

    [[nodiscard]] bool is_compile_time_capable() const noexcept override;
    [[nodiscard]] bool is_constant() const noexcept override;

    // SliceLiteralExpression
    [[nodiscard]] constexpr const std::vector<std::unique_ptr<Expression>>& elements() const {
        return _elements;
    }
    [[nodiscard]] constexpr std::vector<std::unique_ptr<Expression>>& elements() {
        return _elements;
    }

    util::Result<void> validate(Options& options, Scope& scope) override;
};

}  // namespace rain::lang::ast
