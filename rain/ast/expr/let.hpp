#pragma once

#include "rain/ast/expr/expression.hpp"
#include "rain/util/string.hpp"

namespace rain::ast {

class LetExpression : public Expression {
    util::String  _name;
    ExpressionPtr _value;
    bool          _mutable;

  public:
    LetExpression(util::String name, ExpressionPtr value, bool mutable_)
        : _name(name), _value(std::move(value)), _mutable(mutable_) {}

    [[nodiscard]] static std::unique_ptr<LetExpression> alloc(util::String  name,
                                                              ExpressionPtr value, bool mutable_) {
        return std::make_unique<LetExpression>(name, std::move(value), mutable_);
    }

    [[nodiscard]] constexpr ExpressionKind kind() const noexcept override {
        return ExpressionKind::LetExpression;
    }

    [[nodiscard]] bool compile_time_capable() const noexcept override {
        return _value->compile_time_capable();
    }

    [[nodiscard]] util::String         name() const noexcept { return _name; }
    [[nodiscard]] const ExpressionPtr& value() const noexcept { return _value; }
    [[nodiscard]] bool                 mutable_() const noexcept { return _mutable; }
};

}  // namespace rain::ast