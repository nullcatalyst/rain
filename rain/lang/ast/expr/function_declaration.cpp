#include "rain/lang/ast/expr/function.hpp"
#include "rain/lang/ast/var/function.hpp"

namespace rain::lang::ast {

FunctionDeclarationExpression::FunctionDeclarationExpression(
    std::string_view name, ArgumentList arguments, absl::Nonnull<FunctionType*> function_type,
    lex::Location declaration_location)
    : _name(name),
      _arguments(std::move(arguments)),
      _type(function_type),
      _declaration_location(declaration_location) {
    _type->add_ref(*this);
}

util::Result<void> FunctionDeclarationExpression::validate(Options& options, Scope& scope) {
    {
        auto result = _validate_declaration(options, scope);
        FORWARD_ERROR(result);
    }

    return {};
}

void FunctionDeclarationExpression::add_to_scope(Scope& scope) {
    // TODO: Add the proper location
    _variable =
        scope.add_function(nullptr, _type->argument_types(), _name,
                           std::make_unique<FunctionVariable>(_name, _type, lex::Location()));
}

util::Result<void> FunctionDeclarationExpression::_validate_declaration(Options& options,
                                                                        Scope&   scope) {
    for (auto& argument : _arguments) {
        auto result = argument->validate(options, scope);
        FORWARD_ERROR(result);
    }

    // if (_return_type != nullptr) {
    //     auto return_type = _return_type->resolve(options, scope);
    //     FORWARD_ERROR(return_type);

    //     _return_type = std::move(return_type).value();
    // }

    // Scope::TypeList argument_types;
    // argument_types.reserve(_arguments.size());
    // for (const auto& argument : _arguments) {
    //     argument_types.push_back(argument->type());
    // }
    // _type = scope.get_function_type(argument_types, _return_type.get());
    _type = static_cast<FunctionType*>(_type->should_be_replaced_with(scope));

    std::cout << "FunctionDeclarationExpression::_validate_declaration " << _type->display_name()
              << std::endl;

    return {};
}

void FunctionDeclarationExpression::replace_type(absl::Nonnull<Type*> old_type,
                                                 absl::Nonnull<Type*> new_type) {
    // for (auto& argument : _arguments) {
    //     argument->replace_type(old_type, new_type);
    // }

    if (_type == old_type) {
        _type = static_cast<FunctionType*>(new_type);
    }
}

}  // namespace rain::lang::ast
