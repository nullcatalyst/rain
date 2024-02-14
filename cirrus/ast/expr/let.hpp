#pragma once

#include "cirrus/ast/expr/expression.hpp"
#include "cirrus/util/string.hpp"

namespace cirrus::ast {

class LetExpression : public Expression {
    util::String  _name;
    ExpressionPtr _value;
    bool          _mutable;

  public:
    LetExpression(util::String name, ExpressionPtr value, bool mutable_)
        : _name(name), _value(std::move(value)), _mutable(mutable_) {}

    [[nodiscard]] static std::shared_ptr<LetExpression> alloc(util::String  name,
                                                              ExpressionPtr value, bool mutable_) {
        return std::make_shared<LetExpression>(name, std::move(value), mutable_);
    }

    [[nodiscard]] NodeKind kind() const noexcept override { return NodeKind::LetExpression; }

    [[nodiscard]] bool compile_time_capable() const noexcept override {
        return _value->compile_time_capable();
    }

    [[nodiscard]] util::String         name() const noexcept { return _name; }
    [[nodiscard]] const ExpressionPtr& value() const noexcept { return _value; }
    [[nodiscard]] bool                 mutable_() const noexcept { return _mutable; }
};

}  // namespace cirrus::ast
