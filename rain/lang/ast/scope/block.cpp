#include "rain/lang/ast/scope/block.hpp"

namespace rain::lang::ast {

FunctionTypePtr BlockScope::get_function_type(const TypeList& argument_types,
                                              Type*           return_type) noexcept {
    const bool owns_types =
        std::any_of(argument_types.begin(), argument_types.end(),
                    [this](const auto* type) { return _owned_types.contains(type); }) ||
        (return_type != nullptr && _owned_types.contains(return_type));
    if (!owns_types) {
        // If the types are not owned by this scope, then we cannot return a function type, because
        // then when the function type is destroyed, it will create an orphaned function type that
        // will still be owned by this scope.
        return _parent.get_function_type(argument_types, return_type);
    }

    auto key = std::make_tuple(argument_types, return_type);
    if (auto it = _function_types.find(key); it != _function_types.end()) {
        return it->second;
    }

    auto function_type = std::make_shared<FunctionType>(argument_types, return_type);
    _function_types.emplace(key, function_type);
    return function_type;
}

std::optional<TypePtr> BlockScope::find_type(const std::string_view name) const noexcept {
    if (const auto it = _named_types.find(name); it != _named_types.end()) {
        return it->second;
    }
    return _parent.find_type(name);
}

std::optional<VariablePtr> BlockScope::find_method(const TypePtr&         callee_type,
                                                   const std::string_view name) const noexcept {
    if (const auto it = _methods.find(std::make_tuple(callee_type.get(), name));
        it != _methods.end()) {
        return it->second;
    }
    return _parent.find_method(callee_type, name);
}

std::optional<VariablePtr> BlockScope::find_variable(const std::string_view name) const noexcept {
    if (const auto it = _variables.find(name); it != _variables.end()) {
        return it->second;
    }
    return _parent.find_variable(name);
}

void BlockScope::add_type(const std::string_view name, TypePtr type) noexcept {
    Type* const type_ptr = type.get();
    _owned_types.emplace(std::move(type));
    _named_types.emplace(name, type_ptr);
}

void BlockScope::add_method(const TypePtr& callee_type, const std::string_view name,
                            VariablePtr variable) noexcept {
    _methods.emplace(std::make_tuple(callee_type.get(), name), variable);
}

void BlockScope::add_variable(const std::string_view name, VariablePtr variable) noexcept {
    _variables.emplace(name, variable);
}

}  // namespace rain::lang::ast
