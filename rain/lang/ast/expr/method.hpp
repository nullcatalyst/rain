#pragma once

#include <optional>
#include <string_view>

#include "absl/base/nullability.h"
#include "rain/lang/ast/expr/block.hpp"
#include "rain/lang/ast/expr/function.hpp"
#include "rain/util/maybe_owned_ptr.hpp"

namespace rain::lang::ast {

class MethodExpression : public FunctionExpression {
    /** The type of the callee. */
    util::MaybeOwnedPtr<Type> _callee_type;

    [[maybe_unused]] bool _has_self_argument = false;

    lex::Location _callee_type_location;

  public:
    // MethodExpression(util::MaybeOwnedPtr<Type> callee_type, std::string_view name,
    //                  ArgumentList arguments, util::MaybeOwnedPtr<Type> return_type,
    //                  std::unique_ptr<BlockExpression> block, bool has_self_argument,
    //                  lex::Location declaration_location, lex::Location callee_type_location)
    //     : FunctionExpression(name, std::move(arguments), std::move(return_type),
    //     std::move(block),
    //                          declaration_location),
    //       _callee_type(std::move(callee_type)),
    //       _has_self_argument(has_self_argument),
    //       _callee_type_location(callee_type_location) {}
    MethodExpression(util::MaybeOwnedPtr<Type> callee_type, std::string_view name,
                     ArgumentList arguments, absl::Nonnull<FunctionType*> function_type,
                     std::unique_ptr<BlockExpression> block, bool has_self_argument,
                     lex::Location declaration_location, lex::Location callee_type_location)
        : FunctionExpression(name, std::move(arguments), function_type, std::move(block),
                             declaration_location),
          _callee_type(std::move(callee_type)),
          _has_self_argument(has_self_argument),
          _callee_type_location(callee_type_location) {}

    // Expression
    [[nodiscard]] constexpr serial::ExpressionKind kind() const noexcept override {
        return serial::ExpressionKind::Method;
    }
    [[nodiscard]] bool compile_time_capable() const noexcept override {
        // TODO: Should this always return true?
        // Sure, we can always compile the function, but the function could access global mutable
        // variables. We should probably check for that.
        return true;
    }

    [[nodiscard]] absl::Nonnull<Type*> callee_type() const noexcept {
        return _callee_type.get_nonnull();
    }

    [[nodiscard]] bool has_self_argument() const noexcept { return _has_self_argument; }

    [[nodiscard]] lex::Location callee_type_location() const noexcept {
        return _callee_type_location;
    }

    util::Result<void> validate(Options& options, Scope& scope) override;
};

}  // namespace rain::lang::ast
