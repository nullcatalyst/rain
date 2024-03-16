#include "rain/lang/ast/expr/function.hpp"

namespace rain::lang::ast {

util::Result<void> FunctionExpression::validate(Scope& scope) {
    for (auto& argument : _arguments) {
        // auto result = argument->validate(scope);
        // FORWARD_ERROR(result);
    }
    auto result = _block->validate(scope);
    FORWARD_ERROR(result);

    TypeList argument_types;
    argument_types.reserve(_arguments.size());
    for (const auto& argument : _arguments) {
        argument_types.push_back(argument.type);
    }
    _type = scope.get_function_type(argument_types, _return_type);

    return {};
}

}  // namespace rain::lang::ast
