#pragma once

#include <cstdint>
#include <memory>
#include <optional>

#include "absl/base/nullability.h"
#include "rain/lang/ast/expr/block.hpp"
#include "rain/lang/ast/expr/expression.hpp"

namespace rain::lang::ast {

class WhileExpression : public Expression {
    std::unique_ptr<Expression>                     _condition;
    std::unique_ptr<BlockExpression>                _loop;
    std::optional<std::unique_ptr<BlockExpression>> _else;

    absl::Nullable<Type*> _type = nullptr;

  public:
    WhileExpression(std::unique_ptr<Expression> condition, std::unique_ptr<BlockExpression> loop)
        : _condition(std::move(condition)), _loop(std::move(loop)) {}
    WhileExpression(std::unique_ptr<Expression> condition, std::unique_ptr<BlockExpression> loop,
                    std::unique_ptr<BlockExpression> else_)
        : _condition(std::move(condition)), _loop(std::move(loop)), _else(std::move(else_)) {}
    ~WhileExpression() override = default;

    [[nodiscard]] constexpr serial::ExpressionKind kind() const noexcept override {
        return serial::ExpressionKind::While;
    }
    [[nodiscard]] constexpr absl::Nullable<Type*> type() const noexcept override { return _type; }

    [[nodiscard]] constexpr const Expression& condition() const noexcept { return *_condition; }
    [[nodiscard]] constexpr Expression&       condition() noexcept { return *_condition; }

    [[nodiscard]] constexpr const BlockExpression& loop() const noexcept { return *_loop; }
    [[nodiscard]] constexpr BlockExpression&       loop() noexcept { return *_loop; }

    [[nodiscard]] constexpr bool has_else() const noexcept { return _else.has_value(); }
    [[nodiscard]] constexpr const BlockExpression& else_() const noexcept { return *_else.value(); }
    [[nodiscard]] constexpr BlockExpression&       else_() noexcept { return *_else.value(); }

    [[nodiscard]] bool compile_time_capable() const noexcept override;
    util::Result<void> validate(Scope& scope) override;
};

}  // namespace rain::lang::ast
