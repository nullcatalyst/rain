#include "rain/lang/ast/expr/unary_operator.hpp"

#include <optional>
#include <string_view>

#include "absl/strings/str_cat.h"
#include "rain/lang/err/syntax.hpp"
#include "rain/lang/err/unary_operator.hpp"
#include "rain/util/assert.hpp"

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

util::Result<void> UnaryOperatorExpression::validate(Options& options, Scope& scope) {
    {
        auto result = _expression->validate(options, scope);
        FORWARD_ERROR(result);
    }

    const auto method_name = get_operator_method_name(_op);
    IF_DEBUG {
        if (!method_name.has_value()) {
            return ERR_PTR(err::SyntaxError, _op_location,
                           "invalid unary operator; this is an internal error");
        }
    }

    {
        // First check if there is a method that takes self exactly.
        const Scope::TypeList argument_types{_expression->type()};

        _method = scope.find_function(_expression->type(), argument_types, method_name.value());
        if (_method == nullptr) {
            return ERR_PTR(
                err::UnaryOperatorError, _expression->location(), _op_location,
                absl::StrCat("no matching unary operator method found, looking for method named \"",
                             method_name.value(), "\" on type \"", _expression->type()->name(),
                             "\""));
        }
    }

    {
        // TODO: Check if the method can be called with a reference.
    }

    _type = _method->function_type()->return_type();
    return {};
}

}  // namespace rain::lang::ast
