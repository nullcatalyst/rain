#include "rain/lang/ast/expr/cast.hpp"

#include "absl/strings/str_cat.h"
#include "rain/lang/err/binary_operator.hpp"
#include "rain/lang/serial/operator_names.hpp"

namespace rain::lang::ast {

bool CastExpression::compile_time_capable() const noexcept {
    // TODO: Check the operator function as well to determine if is it compile-time capable.
    return _expression->compile_time_capable();
}

util::Result<void> CastExpression::validate(Options& options, Scope& scope) {
    {
        auto result = _expression->validate(options, scope);
        FORWARD_ERROR(result);
    }

    {
        auto result = _type->resolve(options, scope);
        FORWARD_ERROR(result);

        _type = std::move(result).value();
    }

    const Scope::TypeList argument_types{_expression->type()};
    _method = scope.find_method(serial::OperatorNames::CastFrom, _type, argument_types);
    if (_method == nullptr) {
        return ERR_PTR(err::BinaryOperatorError, _expression->location(), _type_location,
                       _op_location,
                       absl::StrCat("no matching cast operator found, looking for method named \"",
                                    serial::OperatorNames::CastFrom, "\" on type \"",
                                    _type->display_name(), "\""));
    }

    _type = _method->function_type()->return_type();
    return {};
}

}  // namespace rain::lang::ast
