#pragma once

#include <string_view>

#include "absl/base/nullability.h"
#include "llvm/ADT/SmallVector.h"
#include "rain/lang/ast/expr/expression.hpp"
#include "rain/lang/ast/var/function.hpp"
#include "rain/lang/serial/expression.hpp"
#include "rain/util/maybe_owned_ptr.hpp"

namespace rain::lang::ast {

using ArgumentList = llvm::SmallVector<util::MaybeOwnedPtr<Variable>, 4>;

class FunctionDeclarationExpression : public Expression {
  protected:
    /** The name of the function. */
    std::string_view _name;

    /** The set of arguments passed to the function. */
    ArgumentList _arguments;

    /**
     * If the function has a name, this will be non-null and will point to the variable that
     * references it.
     */
    absl::Nonnull<FunctionVariable*> _variable = nullptr;

    /** The type of the function. */
    absl::Nonnull<FunctionType*> _type = nullptr;

    /** The location of the declaration. */
    lex::Location _declaration_location;

  public:
    FunctionDeclarationExpression(std::string_view name, ArgumentList arguments,
                                  util::MaybeOwnedPtr<Type> return_type,
                                  lex::Location             declaration_location)
        : _name(name),
          _arguments(std::move(arguments)),
          _return_type(std::move(return_type)),
          _declaration_location(declaration_location) {}

    // Expression
    [[nodiscard]] constexpr serial::ExpressionKind kind() const noexcept override {
        return serial::ExpressionKind::FunctionDeclaration;
    }
    [[nodiscard]] constexpr absl::Nullable<Type*> type() const noexcept override { return _type; }
    [[nodiscard]] constexpr lex::Location         location() const noexcept override {
        return _declaration_location;
    }

    [[nodiscard]] bool compile_time_capable() const noexcept override {
        // TODO: Should this always return true?
        // Sure, we can always compile the function, but the function could access global mutable
        // variables. We should probably check for that.
        return true;
    }

    // FunctionDeclarationExpression
    [[nodiscard]] std::string_view name() const noexcept { return _name; }

    [[nodiscard]] constexpr bool has_return_type() const noexcept {
        return _return_type != nullptr;
    }
    [[nodiscard]] /*constexpr*/ absl::Nullable<Type*> return_type() const noexcept {
        return _return_type.get();
    }

    [[nodiscard]] /*constexpr*/ bool has_arguments() const noexcept { return !_arguments.empty(); }
    [[nodiscard]] constexpr const ArgumentList& arguments() const noexcept { return _arguments; }
    [[nodiscard]] absl::Nullable<FunctionVariable*> variable() const noexcept { return _variable; }

    util::Result<void> validate(Options& options, Scope& scope) override;
    void               add_to_scope(Scope& scope);

  protected:
    util::Result<void> _validate_declaration(Options& options, Scope& scope);
};

}  // namespace rain::lang::ast
