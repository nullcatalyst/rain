#include "rain/lang/ast/expr/binary_operator.hpp"

#include <optional>
#include <string_view>

#include "absl/strings/str_cat.h"
#include "rain/lang/err/binary_operator.hpp"
#include "rain/lang/err/syntax.hpp"
#include "rain/lang/serial/operator_names.hpp"
#include "rain/util/assert.hpp"

namespace rain::lang::ast {

bool BinaryOperatorExpression::compile_time_capable() const noexcept {
    return _lhs->compile_time_capable() && _rhs->compile_time_capable();
}

util::Result<void> BinaryOperatorExpression::validate(Options& options, Scope& scope) {
    {
        auto result = _lhs->validate(options, scope);
        FORWARD_ERROR(result);
    }
    {
        auto result = _rhs->validate(options, scope);
        FORWARD_ERROR(result);
    }

    if (_op == serial::BinaryOperatorKind::Assign) {
        const auto is_reference =
            _lhs->type() != nullptr && _lhs->type()->kind() == serial::TypeKind::Reference;
        const auto is_assignable = _lhs->is_assignable();
        if (!(is_reference || is_assignable)) {
            return ERR_PTR(err::SyntaxError, _lhs->location(),
                           "left-hand side of assignment must be an assignable expression");
        }
        return {};
    }

    const auto method_name = get_binary_operator_name(_op);
    IF_DEBUG {
        if (!method_name.has_value()) {
            return ERR_PTR(err::SyntaxError, _op_location,
                           "invalid binary operator; this is an internal error");
        }
    }

    const Scope::TypeList argument_types{_rhs->type()};
    _method = scope.find_method(method_name.value(), _lhs->type(), argument_types);
    if (_method == nullptr) {
        return ERR_PTR(
            err::BinaryOperatorError, _lhs->location(), _rhs->location(), _op_location,
            absl::StrCat("no matching binary operator method found, looking for method named \"",
                         method_name.value(), "\" on type \"", _lhs->type()->display_name(), "\""));
    }

    _type = _method->function_type()->return_type();
    return {};
}

}  // namespace rain::lang::ast
