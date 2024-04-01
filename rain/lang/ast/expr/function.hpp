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
    FunctionExpression(std::string_view name, ArgumentList arguments,
                       util::MaybeOwnedPtr<Type>        return_type,
                       std::unique_ptr<BlockExpression> block, lex::Location declaration_location)
        : FunctionDeclarationExpression(name, std::move(arguments), std::move(return_type),
                                        declaration_location),
          _block(std::move(block)) {}

    // Expression
    [[nodiscard]] constexpr serial::ExpressionKind kind() const noexcept override {
        return serial::ExpressionKind::Function;
    }
    [[nodiscard]] constexpr absl::Nullable<Type*> type() const noexcept override { return _type; }
    [[nodiscard]] constexpr lex::Location         location() const noexcept override {
        return _declaration_location.merge(_block->location());
    }

    [[nodiscard]] bool compile_time_capable() const noexcept override {
        // TODO: Should this always return true?
        // Sure, we can always compile the function, but the function could access global mutable
        // variables. We should probably check for that.
        return true;
    }

    // FunctionExpression
    [[nodiscard]] std::string_view name() const noexcept { return _name; }

    [[nodiscard]] constexpr bool has_return_type() const noexcept {
        return _return_type != nullptr;
    }
    [[nodiscard]] constexpr absl::Nullable<Type*> return_type() const noexcept {
        return _return_type.get();
    }

    [[nodiscard]] constexpr bool has_arguments() const noexcept { return !_arguments.empty(); }
    [[nodiscard]] constexpr const ArgumentList& arguments() const noexcept { return _arguments; }

    [[nodiscard]] absl::Nonnull<BlockExpression*>   block() const noexcept { return _block.get(); }
    [[nodiscard]] absl::Nullable<FunctionVariable*> variable() const noexcept { return _variable; }

    util::Result<void> validate(Options& options, Scope& scope) override;
};

}  // namespace rain::lang::ast
