#pragma once

#include <memory>
#include <vector>

#include "llvm/ADT/SmallVector.h"
#include "rain/lang/ast/expr/expression.hpp"
#include "rain/lang/ast/scope/block.hpp"

namespace rain::lang::ast {

class BlockExpression : public Expression {
    // TODO: Determine a useful size for this vector.
    llvm::SmallVector<std::unique_ptr<Expression>, 8> _expressions;

    BlockScope _scope;

  public:
    BlockExpression(Scope& parent) : _scope(parent) {}

    void add_expression(std::unique_ptr<Expression> expression) noexcept {
        _expressions.push_back(std::move(expression));
    }

    [[nodiscard]] constexpr const decltype(_expressions)& expressions() const noexcept {
        return _expressions;
    }
};

}  // namespace rain::lang::ast
