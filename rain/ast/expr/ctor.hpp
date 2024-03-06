#pragma once

#include <algorithm>
#include <memory>
#include <vector>

#include "rain/ast/expr/expression.hpp"

namespace rain::ast {

struct CtorExpressionFieldData {
    util::String  name;
    ExpressionPtr value;
};

class CtorExpression : public Expression {
    std::vector<CtorExpressionFieldData> _fields;

  public:
    CtorExpression(TypePtr type, std::vector<CtorExpressionFieldData> fields)
        : _fields(std::move(fields)) {
        _type = std::move(type);
    }

    [[nodiscard]] static std::unique_ptr<CtorExpression> alloc(
        TypePtr type, std::vector<CtorExpressionFieldData> fields) {
        return std::make_unique<CtorExpression>(std::move(type), std::move(fields));
    }

    [[nodiscard]] constexpr ExpressionKind kind() const noexcept override {
        return ExpressionKind::CtorExpression;
    }

    [[nodiscard]] bool compile_time_capable() const noexcept override {
        return std::all_of(_fields.begin(), _fields.end(),
                           [](const CtorExpressionFieldData& field) {
                               return field.value->compile_time_capable();
                           });
    }

    [[nodiscard]] const std::vector<CtorExpressionFieldData>& fields() const& noexcept {
        return _fields;
    }
    [[nodiscard]] std::vector<CtorExpressionFieldData>&& fields() && noexcept {
        return std::move(_fields);
    }
};

}  // namespace rain::ast
