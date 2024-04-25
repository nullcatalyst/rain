#include "rain/lang/ast/expr/unary_operator.hpp"

#include <optional>
#include <string_view>

#include "absl/strings/str_cat.h"
#include "rain/lang/err/syntax.hpp"
#include "rain/lang/err/unary_operator.hpp"
#include "rain/lang/serial/operator_names.hpp"
#include "rain/util/assert.hpp"

namespace rain::lang::ast {

util::Result<void> UnaryOperatorExpression::validate(Options& options, Scope& scope) {
    {
        auto result = _expression->validate(options, scope);
        FORWARD_ERROR(result);
    }

    const auto method_name = get_unary_operator_name(_op);
    IF_DEBUG {
        if (!method_name.has_value()) {
            return ERR_PTR(err::SyntaxError, _op_location,
                           "invalid unary operator; this is an internal error");
        }
    }

    const Scope::TypeList argument_types{};
    _method = scope.find_method(method_name.value(), _expression->type(), argument_types);
    if (_method == nullptr) {
        return ERR_PTR(
            err::UnaryOperatorError, _expression->location(), _op_location,
            absl::StrCat("no matching unary operator method found, looking for method named \"",
                         method_name.value(), "\" on type \"", _expression->type()->display_name(),
                         "\""));
    }

    _type = _method->function_type()->return_type();
    return {};
}

}  // namespace rain::lang::ast
