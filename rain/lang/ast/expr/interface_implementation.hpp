#pragma once

#include <cstdint>
#include <memory>
#include <string_view>

#include "absl/base/nullability.h"
#include "rain/lang/ast/expr/expression.hpp"
#include "rain/lang/ast/var/variable.hpp"

namespace rain::lang::ast {

class InterfaceImplementationExpression : public Expression {
    absl::Nonnull<Type*> _implementee_type;
    absl::Nonnull<Type*> _interface_type;

    std::vector<std::unique_ptr<FunctionExpression>> _methods;

    lex::Location _location;

  public:
    InterfaceImplementationExpression(absl::Nonnull<Type*> implementee_type,
                                      absl::Nonnull<Type*> interface_type,
                                      std::vector<std::unique_ptr<FunctionExpression>> methods,
                                      lex::Location                                    location)
        : _implementee_type(std::move(implementee_type)),
          _interface_type(std::move(interface_type)),
          _methods(std::move(methods)),
          _location(location) {}
    ~InterfaceImplementationExpression() override = default;

    // Expression
    [[nodiscard]] constexpr serial::ExpressionKind kind() const noexcept override {
        return serial::ExpressionKind::Implementation;
    }
    [[nodiscard]] constexpr absl::Nullable<Type*> type() const noexcept override {
        return _value->type();
    }
    [[nodiscard]] constexpr lex::Location location() const noexcept override {
        return _let_location.merge(_value->location());
    }

    // InterfaceImplementationExpression

    util::Result<void> validate(Options& options, Scope& scope) override;
};

}  // namespace rain::lang::ast
