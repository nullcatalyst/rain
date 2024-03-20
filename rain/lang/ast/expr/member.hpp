#pragma once

#include <cstdint>

#include "absl/base/nullability.h"
#include "rain/lang/ast/expr/expression.hpp"

namespace rain::lang::ast {

class CallExpression;

class MemberExpression : public Expression {
    std::unique_ptr<Expression> _lhs;
    std::string_view            _name;
    absl::Nullable<Type*>       _type = nullptr;

    friend class CallExpression;

  public:
    MemberExpression(std::unique_ptr<Expression> lhs, std::string_view name)
        : _lhs(std::move(lhs)), _name(name) {}
    ~MemberExpression() override = default;

    [[nodiscard]] constexpr serial::ExpressionKind kind() const noexcept override {
        return serial::ExpressionKind::Member;
    }
    [[nodiscard]] constexpr absl::Nullable<Type*> type() const noexcept override { return _type; }
    [[nodiscard]] constexpr const std::unique_ptr<Expression>& lhs() const { return _lhs; }
    [[nodiscard]] constexpr std::string_view name() const noexcept { return _name; }

    util::Result<void> validate(Scope& scope) override;
};

}  // namespace rain::lang::ast
