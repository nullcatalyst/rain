#pragma once

#include <optional>
#include <string_view>

#include "absl/base/nullability.h"
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
    /** The name of the function, if it has one. */
    std::optional<std::string_view> _name;

    /** The set of arguments passed to the function. */
    std::vector<FunctionArgument> _arguments;

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

  public:
    FunctionExpression(std::optional<std::string_view> name,
                       std::vector<FunctionArgument> arguments, absl::Nullable<Type*> return_type,
                       std::unique_ptr<BlockExpression> block)
        : _name(std::move(name)),
          _arguments(std::move(arguments)),
          _return_type(return_type),
          _block(std::move(block)) {}

    [[nodiscard]] bool compile_time_capable() const noexcept override {
        // TODO: Should this always return true?
        // Sure, we can always compile the function, but the function could access global mutable
        // variables. We should probably check for that.
        return true;
    }

    [[nodiscard]] constexpr bool   is_named() const noexcept { return _name.has_value(); }
    [[nodiscard]] std::string_view name_or_empty() const noexcept {
        return _name.value_or(std::string_view());
    }

    [[nodiscard]] constexpr bool has_return_type() const noexcept {
        return _return_type != nullptr;
    }
    [[nodiscard]] constexpr absl::Nullable<Type*> return_type() const noexcept {
        return _return_type;
    }

    [[nodiscard]] constexpr bool has_arguments() const noexcept { return !_arguments.empty(); }
    [[nodiscard]] constexpr const std::vector<FunctionArgument>& arguments() const noexcept {
        return _arguments;
    }

    [[nodiscard]] absl::Nullable<BlockExpression*> block() const noexcept { return _block.get(); }

    util::Result<void> validate(Scope& scope) override;
};

}  // namespace rain::lang::ast
