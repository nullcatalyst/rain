#include "rain/lang/ast/expr/method.hpp"

namespace rain::lang::ast {

util::Result<void> MethodExpression::validate(Scope& scope) {
    auto callee_type = _callee_type->resolve(scope);
    FORWARD_ERROR(callee_type);
    // NOTE: Don't replace the `_callee_type` here YET. Wait until AFTER the arguments are
    // validated, because the self argument, if there is one, points to the original type.

    {
        auto result = _validate_without_adding_to_scope(scope);
        FORWARD_ERROR(result);
    }

    Scope::TypeList argument_types;
    argument_types.reserve(_arguments.size());
    for (const auto& argument : _arguments) {
        argument_types.push_back(argument->type());
    }
    _callee_type = std::move(callee_type).value();

    scope.add_method(_callee_type.get_nonnull(), argument_types, _name.value(),
                     std::make_unique<FunctionVariable>(_name.value(), _type));

    return {};
}

}  // namespace rain::lang::ast
