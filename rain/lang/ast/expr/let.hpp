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

    lex::Location _let_location;
    lex::Location _variable_location;

  public:
    LetExpression(std::string_view name, std::unique_ptr<Expression> value,
                  lex::Location let_location, lex::Location variable_location)
        : _name(name),
          _value(std::move(value)),
          _let_location(let_location),
          _variable_location(variable_location) {}
    ~LetExpression() override = default;

    // Expression
    [[nodiscard]] constexpr serial::ExpressionKind kind() const noexcept override {
        return serial::ExpressionKind::Let;
    }
    [[nodiscard]] constexpr absl::Nullable<Type*> type() const noexcept override {
        return _value->type();
    }
    [[nodiscard]] /*constexpr*/ lex::Location location() const noexcept override {
        return _let_location.merge(_value->location());
    }

    // LetExpression
    [[nodiscard]] constexpr std::string_view      name() const noexcept { return _name; }
    [[nodiscard]] /*constexpr*/ const Expression& value() const noexcept { return *_value.get(); }
    [[nodiscard]] /*constexpr*/ Expression&       value() noexcept { return *_value.get(); }
    [[nodiscard]] constexpr absl::Nullable<Variable*> variable() const noexcept {
        return _variable;
    }
    [[nodiscard]] constexpr lex::Location let_location() const noexcept { return _let_location; }
    [[nodiscard]] constexpr lex::Location variable_location() const noexcept {
        return _variable_location;
    }

    util::Result<void> validate(Options& options, Scope& scope) override;
};

}  // namespace rain::lang::ast
