#include "rain/lang/ast/expr/method.hpp"

namespace rain::lang::ast {

util::Result<void> MethodExpression::validate(Scope& scope) {
    auto callee_type = _callee_type->resolve(scope);
    FORWARD_ERROR(callee_type);
    // NOTE: Don't replace the `_callee_type` here YET. Wait until AFTER the arguments are
    // validated, because the self argument, if there is one, points to the original type.

    {
        auto result = _validate_declaration(scope);
        FORWARD_ERROR(result);
    }

    _callee_type = std::move(callee_type).value();

    _variable = scope.add_function(_callee_type.get_nonnull(), _type->argument_types(), _name,
                                   std::make_unique<FunctionVariable>(_name, _type));

    return _block->validate(scope);
}

}  // namespace rain::lang::ast
