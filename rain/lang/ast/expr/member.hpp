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

    lex::Location _member_location;

    friend class CallExpression;

  public:
    MemberExpression(std::unique_ptr<Expression> lhs, std::string_view name,
                     lex::Location member_location)
        : _lhs(std::move(lhs)), _name(name), _member_location(member_location) {}
    ~MemberExpression() override = default;

    // Expression
    [[nodiscard]] constexpr serial::ExpressionKind kind() const noexcept override {
        return serial::ExpressionKind::Member;
    }
    [[nodiscard]] constexpr absl::Nullable<Type*> type() const noexcept override { return _type; }
    [[nodiscard]] constexpr lex::Location         location() const noexcept override {
        return _lhs->location().merge(_member_location);
    }

    // MemberExpression
    [[nodiscard]] constexpr const Expression& lhs() const { return *_lhs; }
    [[nodiscard]] constexpr Expression&       lhs() { return *_lhs; }
    [[nodiscard]] constexpr std::string_view  name() const noexcept { return _name; }
    [[nodiscard]] constexpr lex::Location     member_location() const noexcept {
        return _member_location;
    }

    util::Result<void> validate(Scope& scope) override;
};

}  // namespace rain::lang::ast
