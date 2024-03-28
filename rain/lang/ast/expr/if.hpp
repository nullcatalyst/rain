#pragma once

#include <cstdint>
#include <memory>
#include <optional>

#include "absl/base/nullability.h"
#include "rain/lang/ast/expr/block.hpp"
#include "rain/lang/ast/expr/expression.hpp"

namespace rain::lang::ast {

class IfExpression : public Expression {
    std::unique_ptr<Expression>                     _condition;
    std::unique_ptr<BlockExpression>                _then;
    std::optional<std::unique_ptr<BlockExpression>> _else;

    absl::Nullable<Type*> _type = nullptr;

    lex::Location _if_location;
    lex::Location _else_location;

  public:
    IfExpression(std::unique_ptr<Expression> condition, std::unique_ptr<BlockExpression> then,
                 lex::Location if_location)
        : _condition(std::move(condition)), _then(std::move(then)), _if_location(if_location) {}
    IfExpression(std::unique_ptr<Expression> condition, std::unique_ptr<BlockExpression> then,
                 std::unique_ptr<BlockExpression> else_, lex::Location if_location,
                 lex::Location else_location)
        : _condition(std::move(condition)), _then(std::move(then)), _else(std::move(else_)) {}
    ~IfExpression() override = default;

    // Expression
    [[nodiscard]] constexpr serial::ExpressionKind kind() const noexcept override {
        return serial::ExpressionKind::If;
    }
    [[nodiscard]] constexpr absl::Nullable<Type*> type() const noexcept override { return _type; }
    [[nodiscard]] constexpr lex::Location         location() const noexcept override {
        return has_else() ? _if_location.merge(_else.value()->location())
                                  : _if_location.merge(_then->location());
    }
    [[nodiscard]] bool compile_time_capable() const noexcept override;

    // IfExpression
    [[nodiscard]] constexpr const Expression& condition() const noexcept { return *_condition; }
    [[nodiscard]] constexpr Expression&       condition() noexcept { return *_condition; }

    [[nodiscard]] constexpr const BlockExpression& then() const noexcept { return *_then; }
    [[nodiscard]] constexpr BlockExpression&       then() noexcept { return *_then; }

    [[nodiscard]] constexpr bool has_else() const noexcept { return _else.has_value(); }
    [[nodiscard]] constexpr const BlockExpression& else_() const noexcept { return *_else.value(); }
    [[nodiscard]] constexpr BlockExpression&       else_() noexcept { return *_else.value(); }

    util::Result<void> validate(Scope& scope) override;
};

}  // namespace rain::lang::ast
