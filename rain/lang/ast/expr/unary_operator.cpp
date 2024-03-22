#include "rain/lang/ast/expr/unary_operator.hpp"

#include <optional>
#include <string_view>

#include "absl/strings/str_cat.h"
#include "rain/lang/err/simple.hpp"

namespace rain::lang::ast {

namespace {

std::optional<std::string_view> get_operator_method_name(serial::UnaryOperatorKind op) {
    switch (op) {
        case serial::UnaryOperatorKind::Negative:
            return "__neg__";
        case serial::UnaryOperatorKind::Positive:
            return "__pos__";
        default:
            return std::nullopt;
    }
}

}  // namespace

util::Result<void> UnaryOperatorExpression::validate(Scope& scope) {
    {
        auto result = _rhs->validate(scope);
        FORWARD_ERROR(result);
    }

    const auto method_name = get_operator_method_name(_op);
    if (!method_name.has_value()) {
        return ERR_PTR(err::SimpleError, "invalid unary operator");
    }

    {
        // First check if there is a method that takes self exactly.
        const Scope::TypeList argument_types{_rhs->type()};

        _method = scope.find_function(_rhs->type(), argument_types, method_name.value());
        if (_method == nullptr) {
            return ERR_PTR(
                err::SimpleError,
                absl::StrCat("no matching unary operator method found, looking for method named \"",
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
