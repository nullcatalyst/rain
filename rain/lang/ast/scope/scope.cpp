#include "rain/lang/ast/scope/scope.hpp"

#include "rain/lang/ast/type/function.hpp"
#include "rain/lang/ast/type/type.hpp"
#include "rain/lang/ast/var/function.hpp"
#include "rain/lang/ast/var/variable.hpp"

namespace rain::lang::ast {

absl::Nullable<FunctionType*> Scope::_get_function_type(const TypeList& argument_types,
                                                        Type*           return_type) noexcept {
    const bool owns_types =
        std::any_of(argument_types.begin(), argument_types.end(),
                    [this](const auto* type) { return _owned_types.contains(type); }) ||
        (return_type != nullptr && _owned_types.contains(return_type));
    if (!owns_types) {
        // If the types are not owned by this scope, then we cannot return a function type, because
        // then when the function type is destroyed, it will create an orphaned function type that
        // will still be owned by this scope.
        return nullptr;
    }

    auto key = std::make_tuple(argument_types, return_type);
    if (const auto it = _function_types.find(key); it != _function_types.end()) {
        return it->second;
    }

    auto  function_type     = std::make_unique<FunctionType>(argument_types, return_type);
    auto* function_type_ptr = function_type.get();
    _function_types.emplace(key, function_type_ptr);
    _owned_types.emplace(std::move(function_type));
    return function_type_ptr;
}

absl::Nullable<Type*> Scope::find_type(const std::string_view name) const noexcept {
    if (const auto it = _named_types.find(name); it != _named_types.end()) {
        return it->second;
    }
    return nullptr;
}

absl::Nullable<FunctionVariable*> Scope::find_method(absl::Nonnull<Type*>   callee_type,
                                                     const TypeList&        argument_types,
                                                     const std::string_view name) const noexcept {
    if (const auto it = _method_variables.find(std::make_tuple(callee_type, argument_types, name));
        it != _method_variables.end()) {
        return it->second;
    }
    return nullptr;
}

absl::Nullable<Variable*> Scope::find_variable(const std::string_view name) const noexcept {
    if (const auto it = _named_variables.find(name); it != _named_variables.end()) {
        return it->second;
    }
    return nullptr;
}

void Scope::add_type(const std::string_view name, std::unique_ptr<Type> type) noexcept {
    _named_types.emplace(name, type.get());
    _owned_types.emplace(std::move(type));
}

void Scope::add_method(absl::Nonnull<Type*> callee_type, const TypeList& argument_types,
                       const std::string_view            name,
                       std::unique_ptr<FunctionVariable> method) noexcept {
    _method_variables.emplace(std::make_tuple(callee_type, argument_types, name), method.get());
    _owned_variables.emplace(std::move(method));
}

void Scope::add_variable(const std::string_view name, std::unique_ptr<Variable> variable) noexcept {
    _named_variables.emplace(name, variable.get());
    _owned_variables.emplace(std::move(variable));
}
}  // namespace rain::lang::ast
