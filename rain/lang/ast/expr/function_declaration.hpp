#pragma once

#include <string_view>

#include "absl/base/nullability.h"
#include "llvm/ADT/SmallVector.h"
#include "rain/lang/ast/expr/expression.hpp"
#include "rain/lang/ast/scope/block.hpp"
#include "rain/lang/ast/var/function.hpp"
#include "rain/lang/serial/expression.hpp"
#include "rain/util/maybe_owned_ptr.hpp"

namespace rain::lang::ast {

using ArgumentList = llvm::SmallVector<util::MaybeOwnedPtr<Variable>, 4>;

class FunctionDeclarationExpression : public Expression {
  protected:
    /** The set of arguments passed to the function. */
    ArgumentList _arguments;

    /**
     * If the function has a name, this will be non-null and will point to the variable that
     * references it.
     */
    absl::Nullable<FunctionVariable*> _variable = nullptr;

    /**
     * The type of the function.
     *
     * This may be redundant, as the type of the function is also stored in the function variable.
     * But there might not be a variable for the function if it is an unnamed anonymous function.
     */
    absl::Nonnull<FunctionType*> _type;

    /** The scope which owns the argument variables. */
    BlockScope _scope;

    /** The location of the declaration. */
    lex::Location _declaration_location;
    lex::Location _return_type_location;

    FunctionDeclarationExpression(FunctionDeclarationExpression&&);

  public:
    FunctionDeclarationExpression(Scope& parent, absl::Nullable<FunctionVariable*> variable,
                                  ArgumentList                 arguments,
                                  absl::Nonnull<FunctionType*> function_type,
                                  lex::Location                declaration_location,
                                  lex::Location                return_type_location);
    FunctionDeclarationExpression(const FunctionDeclarationExpression&)            = delete;
    FunctionDeclarationExpression& operator=(const FunctionDeclarationExpression&) = delete;
    FunctionDeclarationExpression& operator=(FunctionDeclarationExpression&&)      = delete;
    ~FunctionDeclarationExpression() override                                      = default;

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
    [[nodiscard]] std::string_view name() const noexcept {
        return _variable != nullptr ? _variable->name() : std::string_view();
    }
    [[nodiscard]] constexpr absl::Nonnull<FunctionType*> function_type() const noexcept {
        return _type;
    }
    [[nodiscard]] /*constexpr*/ bool has_arguments() const noexcept { return !_arguments.empty(); }
    [[nodiscard]] constexpr const ArgumentList& arguments() const noexcept { return _arguments; }
    [[nodiscard]] absl::Nullable<FunctionVariable*> variable() const noexcept { return _variable; }
    [[nodiscard]] Scope&                            scope() noexcept { return _scope; }
    [[nodiscard]] const Scope&                      scope() const noexcept { return _scope; }

    util::Result<void> validate(Options& options, Scope& scope) override;

  protected:
    util::Result<void> _validate_declaration(Options& options, Scope& scope);
};

}  // namespace rain::lang::ast
