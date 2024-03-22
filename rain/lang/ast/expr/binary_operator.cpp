#include "rain/lang/ast/expr/binary_operator.hpp"

#include <optional>
#include <string_view>

#include "absl/strings/str_cat.h"
#include "rain/lang/err/simple.hpp"

namespace rain::lang::ast {

namespace {

std::optional<std::string_view> get_operator_method_name(serial::BinaryOperatorKind op) {
    switch (op) {
        case serial::BinaryOperatorKind::Add:
            return "__add__";
        case serial::BinaryOperatorKind::Subtract:
            return "__sub__";
        case serial::BinaryOperatorKind::Multiply:
            return "__mul__";
        case serial::BinaryOperatorKind::Divide:
            return "__div__";
        case serial::BinaryOperatorKind::Modulo:
            return "__rem__";
        case serial::BinaryOperatorKind::Equal:
            return "__eq__";
        case serial::BinaryOperatorKind::NotEqual:
            return "__ne__";
        case serial::BinaryOperatorKind::Less:
            return "__lt__";
        case serial::BinaryOperatorKind::LessEqual:
            return "__le__";
        case serial::BinaryOperatorKind::Greater:
            return "__gt__";
        case serial::BinaryOperatorKind::GreaterEqual:
            return "__ge__";
        default:
            return std::nullopt;
    }
}

}  // namespace

util::Result<void> BinaryOperatorExpression::validate(Scope& scope) {
    {
        auto result = _lhs->validate(scope);
        FORWARD_ERROR(result);
    }
    {
        auto result = _rhs->validate(scope);
        FORWARD_ERROR(result);
    }

    const auto method_name = get_operator_method_name(_op);
    if (!method_name.has_value()) {
        return ERR_PTR(err::SimpleError, "invalid binary operator");
    }

    {
        // First check if there is a method that takes self exactly.
        const Scope::TypeList argument_types{_lhs->type(), _rhs->type()};

        _method = scope.find_function(_lhs->type(), argument_types, method_name.value());
        if (_method == nullptr) {
            return ERR_PTR(
                err::SimpleError,
                absl::StrCat(
                    "no matching binary operator method found, looking for method named \"",
                    method_name.value(), "\""));
        }
    }

    {
        // TODO: Check if the method can be called with a reference.
    }

    _type = _method->function_type()->return_type();
    return {};
}

}  // namespace rain::lang::ast
