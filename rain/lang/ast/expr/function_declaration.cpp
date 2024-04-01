#include "rain/lang/ast/expr/function.hpp"
#include "rain/lang/ast/var/function.hpp"

namespace rain::lang::ast {

util::Result<void> FunctionDeclarationExpression::validate(Options& options, Scope& scope) {
    {
        auto result = _validate_declaration(options, scope);
        FORWARD_ERROR(result);
    }

    _variable = scope.add_function(nullptr, _type->argument_types(), _name,
                                   std::make_unique<FunctionVariable>(_name, _type));

    return {};
}

util::Result<void> FunctionDeclarationExpression::_validate_declaration(Options& options,
                                                                        Scope&   scope) {
    for (auto& argument : _arguments) {
        auto result = argument->validate(options, scope);
        FORWARD_ERROR(result);
    }

    if (_return_type != nullptr) {
        auto return_type = _return_type->resolve(scope);
        FORWARD_ERROR(return_type);

        _return_type = std::move(return_type).value();
    }

    Scope::TypeList argument_types;
    argument_types.reserve(_arguments.size());
    for (const auto& argument : _arguments) {
        argument_types.push_back(argument->type());
    }
    _type = scope.get_function_type(argument_types, _return_type.get());

    return {};
}

}  // namespace rain::lang::ast
