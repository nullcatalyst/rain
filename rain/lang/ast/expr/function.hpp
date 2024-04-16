#pragma once

#include <optional>
#include <string_view>

#include "absl/base/nullability.h"
#include "rain/lang/ast/expr/block.hpp"
#include "rain/lang/ast/expr/function_declaration.hpp"
#include "rain/lang/ast/var/function.hpp"
#include "rain/lang/serial/expression.hpp"
#include "rain/util/maybe_owned_ptr.hpp"

namespace rain::lang::ast {

class FunctionExpression : public FunctionDeclarationExpression {
  protected:
    /** The function body. */
    std::unique_ptr<BlockExpression> _block;

  public:
    FunctionExpression(absl::Nullable<FunctionVariable*> variable, ArgumentList arguments,
                       absl::Nonnull<FunctionType*>     function_type,
                       std::unique_ptr<BlockExpression> block, lex::Location declaration_location,
                       lex::Location return_type_location);

    // Expression
    [[nodiscard]] constexpr serial::ExpressionKind kind() const noexcept override {
        return serial::ExpressionKind::Function;
    }
    [[nodiscard]] constexpr absl::Nullable<Type*> type() const noexcept override { return _type; }
    [[nodiscard]] /*constexpr*/ lex::Location     location() const noexcept override {
        return _declaration_location.merge(_block->location());
    }

    [[nodiscard]] bool compile_time_capable() const noexcept override {
        // TODO: Should this always return true?
        // Sure, we can always compile the function, but the function could access global mutable
        // variables. We should probably check for that.
        return true;
    }

    // FunctionExpression
    [[nodiscard]] std::string_view name() const noexcept {
        return _variable != nullptr ? _variable->name() : std::string_view();
    }

    [[nodiscard]] /*constexpr*/ bool has_arguments() const noexcept { return !_arguments.empty(); }
    [[nodiscard]] constexpr const ArgumentList& arguments() const noexcept { return _arguments; }

    [[nodiscard]] absl::Nonnull<BlockExpression*>   block() const noexcept { return _block.get(); }
    [[nodiscard]] absl::Nullable<FunctionVariable*> variable() const noexcept { return _variable; }

    util::Result<void> validate(Options& options, Scope& scope) override;
};

}  // namespace rain::lang::ast
