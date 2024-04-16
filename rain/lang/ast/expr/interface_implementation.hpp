#pragma once

#include <cstdint>
#include <memory>
#include <string_view>

#include "absl/base/nullability.h"
#include "rain/lang/ast/expr/expression.hpp"
#include "rain/lang/ast/expr/function.hpp"
#include "rain/lang/ast/var/variable.hpp"

namespace rain::lang::ast {

class InterfaceImplementationExpression : public Expression {
    absl::Nonnull<Type*> _implementer_type;
    absl::Nonnull<Type*> _interface_type;

    std::vector<std::unique_ptr<FunctionExpression>> _methods;

    lex::Location _location;

  public:
    InterfaceImplementationExpression(absl::Nonnull<Type*> implementer_type,
                                      absl::Nonnull<Type*> interface_type,
                                      std::vector<std::unique_ptr<FunctionExpression>> methods,
                                      lex::Location                                    location);
    ~InterfaceImplementationExpression() override = default;

    // Expression
    [[nodiscard]] constexpr serial::ExpressionKind kind() const noexcept override {
        return serial::ExpressionKind::Implementation;
    }
    [[nodiscard]] constexpr absl::Nullable<Type*> type() const noexcept override { return nullptr; }
    [[nodiscard]] constexpr lex::Location location() const noexcept override { return _location; }

    // InterfaceImplementationExpression
    [[nodiscard]] constexpr auto implementer_type() const noexcept { return _implementer_type; }
    [[nodiscard]] constexpr auto interface_type() const noexcept { return _interface_type; }
    [[nodiscard]] constexpr const auto& methods() const noexcept { return _methods; }

    util::Result<void> validate(Options& options, Scope& scope) override;
};

}  // namespace rain::lang::ast
