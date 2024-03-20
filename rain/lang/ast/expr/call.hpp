#pragma once

#include <cstdint>

#include "absl/base/nullability.h"
#include "rain/lang/ast/expr/expression.hpp"

namespace rain::lang::ast {

class CallExpression : public Expression {
    std::unique_ptr<Expression>                       _callee;
    llvm::SmallVector<std::unique_ptr<Expression>, 4> _arguments;

    absl::Nullable<Type*> _type = nullptr;

  public:
    CallExpression(std::unique_ptr<Expression>                       callee,
                   llvm::SmallVector<std::unique_ptr<Expression>, 4> arguments)
        : _callee(std::move(callee)), _arguments(std::move(arguments)) {}
    ~CallExpression() override = default;

    [[nodiscard]] constexpr serial::ExpressionKind kind() const noexcept override {
        return serial::ExpressionKind::Call;
    }
    [[nodiscard]] constexpr absl::Nullable<Type*> type() const noexcept override { return _type; }
    [[nodiscard]] constexpr const std::unique_ptr<Expression>& callee() const { return _callee; }
    [[nodiscard]] constexpr const llvm::SmallVector<std::unique_ptr<Expression>, 4>& arguments()
        const {
        return _arguments;
    }

    util::Result<void> validate(Scope& scope) override;
};

}  // namespace rain::lang::ast
