#pragma once

#include <cstdint>

#include "absl/base/nullability.h"
#include "rain/lang/ast/expr/expression.hpp"

namespace rain::lang::ast {

class CallExpression : public Expression {
    std::unique_ptr<Expression>                       _callee;
    llvm::SmallVector<std::unique_ptr<Expression>, 4> _arguments;

    absl::Nullable<FunctionVariable*> _function = nullptr;
    absl::Nullable<Type*>             _type     = nullptr;

    lex::Location _arguments_location;

  public:
    CallExpression(std::unique_ptr<Expression>                       callee,
                   llvm::SmallVector<std::unique_ptr<Expression>, 4> arguments,
                   lex::Location                                     arguments_location)
        : _callee(std::move(callee)),
          _arguments(std::move(arguments)),
          _arguments_location(arguments_location) {}
    ~CallExpression() override = default;

    // Expression
    [[nodiscard]] constexpr serial::ExpressionKind kind() const noexcept override {
        return serial::ExpressionKind::Call;
    }
    [[nodiscard]] constexpr absl::Nullable<Type*> type() const noexcept override { return _type; }
    [[nodiscard]] constexpr lex::Location         location() const noexcept override {
        return _callee->location().merge(_arguments_location);
    }
    [[nodiscard]] bool compile_time_capable() const noexcept override;

    // CallExpression
    [[nodiscard]] constexpr const std::unique_ptr<Expression>& callee() const { return _callee; }
    [[nodiscard]] constexpr const llvm::SmallVector<std::unique_ptr<Expression>, 4>& arguments()
        const {
        return _arguments;
    }
    [[nodiscard]] absl::Nullable<FunctionVariable*> function() const noexcept { return _function; }
    [[nodiscard]] constexpr lex::Location           arguments_location() const noexcept {
        return _arguments_location;
    }

    util::Result<void> validate(Options& options, Scope& scope) override;
};

}  // namespace rain::lang::ast
