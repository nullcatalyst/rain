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

  public:
    MethodExpression(util::MaybeOwnedPtr<Type> callee_type, std::string_view name,
                     llvm::SmallVector<absl::Nonnull<Variable*>, 4> arguments,
                     util::MaybeOwnedPtr<Type> return_type, std::unique_ptr<BlockExpression> block,
                     bool has_self_argument)
        : FunctionExpression(name, std::move(arguments), std::move(return_type), std::move(block)),
          _callee_type(std::move(callee_type)),
          _has_self_argument(has_self_argument) {}

    [[nodiscard]] bool compile_time_capable() const noexcept override {
        // TODO: Should this always return true?
        // Sure, we can always compile the function, but the function could access global mutable
        // variables. We should probably check for that.
        return true;
    }

    [[nodiscard]] absl::Nonnull<Type*> callee_type() const noexcept {
        return _callee_type.get_nonnull();
    }

    util::Result<void> validate(Scope& scope) override;
};

}  // namespace rain::lang::ast
