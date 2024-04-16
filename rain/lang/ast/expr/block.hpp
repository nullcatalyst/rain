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

    absl::Nullable<Type*> _type = nullptr;
    lex::Location         _location;

  public:
    // BlockExpression is the only expression that has to be created prior to finishing parsing,
    // because the scope is needed to have already been created in order to parse the expressions in
    // the block.
    explicit BlockExpression(Scope& parent) : _scope(parent) {}

    // Expression
    [[nodiscard]] constexpr serial::ExpressionKind kind() const noexcept override {
        return serial::ExpressionKind::Block;
    }
    [[nodiscard]] constexpr absl::Nullable<Type*> type() const noexcept override { return _type; }
    [[nodiscard]] constexpr lex::Location location() const noexcept override { return _location; }

    // BlockExpression
    [[nodiscard]] constexpr const llvm::SmallVector<std::unique_ptr<Expression>, 8>& expressions()
        const noexcept {
        return _expressions;
    }
    [[nodiscard]] constexpr BlockScope& scope() noexcept { return _scope; }

    void add_expression(std::unique_ptr<Expression> expression) {
        _expressions.push_back(std::move(expression));
    }

    void set_location(const lex::Location location) noexcept { _location = location; }

    [[nodiscard]] bool compile_time_capable() const noexcept override;
    util::Result<void> validate(Options& options, Scope& scope) override;
};

}  // namespace rain::lang::ast
