#pragma once

#include <optional>
#include <string_view>

#include "absl/base/nullability.h"
#include "rain/lang/ast/expr/expression.hpp"
#include "rain/lang/ast/type/type.hpp"
#include "rain/lang/serial/expression.hpp"
#include "rain/util/maybe_owned_ptr.hpp"

namespace rain::lang::ast {

class TypeExpression : public Expression {
    absl::Nonnull<Type*> _declare_type;

  public:
    TypeExpression(absl::Nonnull<Type*> declare_type) : _declare_type(declare_type) {}

    [[nodiscard]] constexpr serial::ExpressionKind kind() const noexcept override {
        return serial::ExpressionKind::Type;
    }
    [[nodiscard]] constexpr absl::Nullable<Type*> type() const noexcept override { return nullptr; }
    [[nodiscard]] constexpr absl::Nonnull<Type*>  declare_type() const noexcept {
        return _declare_type;
    }

    [[nodiscard]] bool compile_time_capable() const noexcept override { return true; }

    util::Result<void> validate(Scope& scope) override { return {}; }
};

}  // namespace rain::lang::ast
