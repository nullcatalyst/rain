#include "rain/lang/ast/expr/function.hpp"
#include "rain/lang/ast/var/function.hpp"

namespace rain::lang::ast {

FunctionDeclarationExpression::FunctionDeclarationExpression(
    Scope& parent, absl::Nullable<FunctionVariable*> variable,
    llvm::SmallVector<std::unique_ptr<Variable>, 4> arguments,
    absl::Nonnull<FunctionType*> function_type, lex::Location declaration_location,
    lex::Location return_type_location)
    : _variable(variable),
      _type(function_type),
      _scope(std::make_unique<BlockScope>(parent)),
      _declaration_location(declaration_location),
      _return_type_location(return_type_location) {
    for (auto& argument : arguments) {
        assert(argument != nullptr && "argument is null");

        // Get the argument name _BEFORE_ moving the argument into the _arguments vector. THe order
        // of evaluaation of the arguments is not guaranteed (as seen by this crashing on windows
        // with a null pointer dereference, since the value has already been moved out of the smart
        // pointer just before the name method is called).
        const auto argument_name = argument->name();
        _arguments.emplace_back(_scope->add_variable(std::move(argument)));
    }
}

FunctionDeclarationExpression::FunctionDeclarationExpression(FunctionDeclarationExpression&& other)
    : _arguments(std::move(other._arguments)),
      _variable(other._variable),
      _type(other._type),
      _scope{std::move(other._scope)},
      _declaration_location(other._declaration_location),
      _return_type_location(other._return_type_location) {}

util::Result<void> FunctionDeclarationExpression::validate(Options& options, Scope& scope) {
    {
        auto result = _validate_declaration(options, scope);
        FORWARD_ERROR(result);
    }

    return {};
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
