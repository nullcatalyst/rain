#include "rain/lang/ast/type/function.hpp"

#include "rain/lang/ast/scope/scope.hpp"

namespace rain::lang::ast {

util::Result<absl::Nonnull<Type*>> FunctionType::resolve(Scope& scope) {
    Scope::TypeList resolved_argument_types;
    resolved_argument_types.reserve(_argument_types.size());
    for (auto* argument : _argument_types) {
        auto argument_type = argument->resolve(scope);
        FORWARD_ERROR(argument_type);

        resolved_argument_types.push_back(std::move(argument_type).value());
    }

    absl::Nullable<Type*> resolved_return_type = nullptr;
    if (_return_type != nullptr) {
        auto return_type = _return_type->resolve(scope);
        FORWARD_ERROR(return_type);

        resolved_return_type = std::move(return_type).value();
    }

    return scope.get_function_type(resolved_argument_types, resolved_return_type);
}

}  // namespace rain::lang::ast
