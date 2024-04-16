#include "rain/lang/ast/expr/interface_implementation.hpp"

#include "rain/lang/ast/type/interface.hpp"
#include "rain/lang/err/syntax.hpp"

namespace rain::lang::ast {

InterfaceImplementationExpression::InterfaceImplementationExpression(
    absl::Nonnull<Type*> implementer_type, absl::Nonnull<Type*> interface_type,
    std::vector<std::unique_ptr<FunctionExpression>> methods, lex::Location location)
    : _implementer_type(implementer_type),
      _interface_type(interface_type),
      _methods(std::move(methods)),
      _location(location) {}

util::Result<void> InterfaceImplementationExpression::validate(Options& options, Scope& scope) {
    {
        auto implementer_type = _implementer_type->resolve(options, scope);
        FORWARD_ERROR(implementer_type);
    }

    InterfaceType* interface_type = nullptr;
    {
        auto interface_type_result = _interface_type->resolve(options, scope);
        FORWARD_ERROR(interface_type_result);

        interface_type = static_cast<InterfaceType*>(std::move(interface_type_result).value());
    }

    absl::flat_hash_set<Scope::FunctionVariableKey> function_keys;
    for (const auto& method : _methods) {
        auto result = method->validate(options, scope);
        FORWARD_ERROR(result);

        const auto& type = method->function_type();
        function_keys.emplace(Scope::FunctionVariableKey{method->name(), type->callee_type(),
                                                         type->argument_types()});
    }

    for (const auto& declaration : interface_type->methods()) {
        const auto& type = declaration->function_type();
        const auto  key  = Scope::FunctionVariableKey{declaration->name(), type->callee_type(),
                                                    type->argument_types()};
        if (function_keys.contains(key)) {
            continue;
        }

        return ERR_PTR(err::SyntaxError, _location,
                       absl::StrCat("interface '", interface_type->name(),
                                    "' does not contain a function named '", declaration->name(),
                                    "' with type '", type->display_name(), "'"));
    }

    return {};
}

}  // namespace rain::lang::ast
