#pragma once

#include <vector>

#include "rain/ast/expr/expression.hpp"

namespace rain::ast {

class BlockExpression : public Expression {
    std::vector<ExpressionPtr> _expressions;

  public:
    BlockExpression(std::vector<ExpressionPtr> expressions)
        : _expressions(std::move(expressions)) {}

    [[nodiscard]] static std::unique_ptr<BlockExpression> alloc(
        std::vector<ExpressionPtr> expressions) {
        return std::make_unique<BlockExpression>(std::move(expressions));
    }

    [[nodiscard]] constexpr ExpressionKind kind() const noexcept override {
        return ExpressionKind::BlockExpression;
    }

    [[nodiscard]] bool compile_time_capable() const noexcept override;

    [[nodiscard]] const std::vector<ExpressionPtr>& expressions() const& noexcept {
        return _expressions;
    }
    [[nodiscard]] std::vector<ExpressionPtr>&& expressions() && noexcept {
        return std::move(_expressions);
    }
};

}  // namespace rain::ast