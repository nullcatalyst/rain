#pragma once

#include <vector>

#include "cirrus/ast/expr/expression.hpp"

namespace cirrus::ast {

class BlockExpression : public Expression {
    std::vector<ExpressionPtr> _expressions;

  public:
    BlockExpression(std::vector<ExpressionPtr> expressions)
        : _expressions(std::move(expressions)) {}

    [[nodiscard]] static std::shared_ptr<BlockExpression> alloc(
        std::vector<ExpressionPtr> expressions) {
        return std::make_shared<BlockExpression>(std::move(expressions));
    }

    [[nodiscard]] NodeKind kind() const noexcept override { return NodeKind::BlockExpression; }

    [[nodiscard]] bool compile_time_capable() const noexcept override;

    [[nodiscard]] const std::vector<ExpressionPtr>& expressions() const& noexcept {
        return _expressions;
    }
    [[nodiscard]] std::vector<ExpressionPtr>&& expressions() && noexcept {
        return std::move(_expressions);
    }
};

}  // namespace cirrus::ast
