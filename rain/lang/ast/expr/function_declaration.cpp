#include "rain/lang/ast/expr/function.hpp"
#include "rain/lang/ast/var/function.hpp"

namespace rain::lang::ast {

FunctionDeclarationExpression::FunctionDeclarationExpression(
    std::string_view name, ArgumentList arguments, absl::Nonnull<FunctionType*> function_type,
    lex::Location declaration_location)
    : _name(name),
      _arguments(std::move(arguments)),
      _type(function_type),
      _declaration_location(declaration_location) {}

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
    {
        auto result = _type->resolve(options, scope);
        FORWARD_ERROR(result);

        _type = static_cast<FunctionType*>(std::move(result).value());
    }

    for (auto& argument : _arguments) {
        auto result = argument->validate(options, scope);
        FORWARD_ERROR(result);
    }

    return {};
}

}  // namespace rain::lang::ast
