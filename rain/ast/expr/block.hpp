#pragma once

#include <vector>

#include "rain/ast/expr/expression.hpp"
#include "rain/ast/expr/statement.hpp"

namespace rain::ast {

class BlockExpression : public Expression {
    std::vector<StatementPtr> _statements;
    ExpressionPtr             _expression;

  public:
    BlockExpression(std::vector<StatementPtr> statements, ExpressionPtr expression)
        : _statements(std::move(statements)), _expression(std::move(expression)) {}

    [[nodiscard]] static std::unique_ptr<BlockExpression> alloc(
        std::vector<StatementPtr> statements, ExpressionPtr expression) {
        return std::make_unique<BlockExpression>(std::move(statements), std::move(expression));
    }

    [[nodiscard]] constexpr ExpressionKind kind() const noexcept override {
        return ExpressionKind::BlockExpression;
    }

    [[nodiscard]] bool compile_time_capable() const noexcept override;

    [[nodiscard]] const std::vector<StatementPtr>& statements() const& noexcept {
        return _statements;
    }
    [[nodiscard]] std::vector<StatementPtr>&& statements() && noexcept {
        return std::move(_statements);
    }
    [[nodiscard]] const ExpressionPtr& expression() const& noexcept { return _expression; }
    [[nodiscard]] ExpressionPtr&&      expression() && noexcept { return std::move(_expression); }
};

using BlockPtr = std::unique_ptr<BlockExpression>;

}  // namespace rain::ast
