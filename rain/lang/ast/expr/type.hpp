#pragma once

#include <optional>
#include <string_view>

#include "absl/base/nullability.h"
#include "rain/lang/ast/expr/expression.hpp"
#include "rain/lang/ast/type/type.hpp"
#include "rain/lang/serial/expression.hpp"

namespace rain::lang::ast {

class TypeExpression : public Expression {
    absl::Nonnull<Type*> _declare_type;

    lex::Location _location;

  public:
    TypeExpression(absl::Nonnull<Type*> declare_type) : _declare_type(declare_type) {}
    TypeExpression(absl::Nonnull<Type*> declare_type, lex::Location location)
        : _declare_type(declare_type), _location(location) {}

    // Expression
    [[nodiscard]] constexpr serial::ExpressionKind kind() const noexcept override {
        return serial::ExpressionKind::Type;
    }
    [[nodiscard]] constexpr absl::Nullable<Type*> type() const noexcept override { return nullptr; }
    [[nodiscard]] constexpr lex::Location location() const noexcept override { return _location; }

    [[nodiscard]] constexpr bool is_compile_time_capable() const noexcept override { return true; }

    // TypeExpression
    [[nodiscard]] constexpr const Type& declare_type() const noexcept { return *_declare_type; }
    [[nodiscard]] constexpr Type&       declare_type() noexcept { return *_declare_type; }

    util::Result<void> validate(Options& options, Scope& scope) override;
};

}  // namespace rain::lang::ast
