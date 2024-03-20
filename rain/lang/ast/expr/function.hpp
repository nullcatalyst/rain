#pragma once

#include <optional>
#include <string_view>

#include "absl/base/nullability.h"
#include "llvm/ADT/SmallVector.h"
#include "rain/lang/ast/expr/block.hpp"
#include "rain/lang/ast/expr/expression.hpp"
#include "rain/lang/ast/var/function.hpp"
#include "rain/lang/serial/expression.hpp"
#include "rain/util/maybe_owned_ptr.hpp"

namespace rain::lang::ast {

struct FunctionArgument {
    std::string_view          name;
    util::MaybeOwnedPtr<Type> type;
};

class FunctionExpression : public Expression {
  public:
    using ArgumentList = llvm::SmallVector<absl::Nonnull<Variable*>, 4>;

  protected:
    /** The name of the function, if it has one. */
    std::optional<std::string_view> _name;

    /** The set of arguments passed to the function. */
    ArgumentList _arguments;

    /**
     * The return type of the function, or null if the function does not return a value.
     *
     * This is a "maybe owned pointer" because the return type may be a type that is not yet defined
     * (eg: an unresolved type referenced only by name), in which case this instance owns the
     * pointer. But if the type is already defined, then this instance does not own the pointer,
     * since the type will be owned by the scope.
     */
    util::MaybeOwnedPtr<Type> _return_type;

    /** The function body. */
    std::unique_ptr<BlockExpression> _block;

    /**
     * If the function has a name, this will be non-null and will point to the variable that
     * references it.
     */
    absl::Nullable<FunctionVariable*> _variable = nullptr;

    /** The type of the function. */
    absl::Nullable<FunctionType*> _type = nullptr;

  public:
    FunctionExpression(std::optional<std::string_view> name, ArgumentList arguments,
                       util::MaybeOwnedPtr<Type>        return_type,
                       std::unique_ptr<BlockExpression> block)
        : _name(std::move(name)),
          _arguments(std::move(arguments)),
          _return_type(std::move(return_type)),
          _block(std::move(block)) {}

    [[nodiscard]] constexpr serial::ExpressionKind kind() const noexcept override {
        return serial::ExpressionKind::Function;
    }
    [[nodiscard]] constexpr absl::Nullable<Type*> type() const noexcept override { return _type; }
    [[nodiscard]] constexpr bool   is_named() const noexcept { return _name.has_value(); }
    [[nodiscard]] std::string_view name_or_empty() const noexcept {
        return _name.value_or(std::string_view());
    }

    [[nodiscard]] constexpr bool has_return_type() const noexcept {
        return _return_type != nullptr;
    }
    [[nodiscard]] constexpr absl::Nullable<Type*> return_type() const noexcept {
        return _return_type.get();
    }

    [[nodiscard]] constexpr bool has_arguments() const noexcept { return !_arguments.empty(); }
    [[nodiscard]] constexpr const ArgumentList& arguments() const noexcept { return _arguments; }

    [[nodiscard]] absl::Nonnull<BlockExpression*> block() const noexcept { return _block.get(); }

    [[nodiscard]] absl::Nullable<FunctionVariable*> variable() const noexcept { return _variable; }

    [[nodiscard]] bool compile_time_capable() const noexcept override {
        // TODO: Should this always return true?
        // Sure, we can always compile the function, but the function could access global mutable
        // variables. We should probably check for that.
        return true;
    }

    util::Result<void> validate(Scope& scope) override;

  protected:
    util::Result<void> _validate_without_adding_to_scope(Scope& scope);
};

}  // namespace rain::lang::ast
