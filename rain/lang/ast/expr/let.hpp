#pragma once

#include <cstdint>
#include <memory>
#include <string_view>

#include "absl/base/nullability.h"
#include "rain/lang/ast/expr/expression.hpp"
#include "rain/lang/ast/var/variable.hpp"

namespace rain::lang::ast {

class LetExpression : public Expression {
    std::string_view _name;

    std::unique_ptr<Expression> _value;
    absl::Nullable<Variable*>   _variable = nullptr;

  public:
    LetExpression(std::string_view name, std::unique_ptr<Expression> value)
        : _name(name), _value(std::move(value)) {}
    ~LetExpression() override = default;

    [[nodiscard]] constexpr serial::ExpressionKind kind() const noexcept override {
        return serial::ExpressionKind::Let;
    }
    [[nodiscard]] constexpr absl::Nullable<Type*> type() const noexcept override {
        return _value->type();
    }
    [[nodiscard]] constexpr std::string_view  name() const noexcept { return _name; }
    [[nodiscard]] constexpr const Expression& value() const noexcept { return *_value.get(); }
    [[nodiscard]] constexpr Expression&       value() noexcept { return *_value.get(); }
    [[nodiscard]] constexpr absl::Nullable<Variable*> variable() const noexcept {
        return _variable;
    }

    util::Result<void> validate(Scope& scope) override;
};

}  // namespace rain::lang::ast
