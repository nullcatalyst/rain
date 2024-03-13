#include "rain/lang/ast/scope/module.hpp"

namespace rain::lang::ast {

FunctionType* ModuleScope::get_function_type(const TypeList&      argument_types,
                                             std::optional<Type*> return_type) noexcept {
    if (auto* type = Scope::get_function_type(argument_types, return_type); type != nullptr) {
        return type;
    }
    return _builtin.get_function_type(argument_types, return_type);
}

std::optional<Type*> ModuleScope::find_type(const std::string_view name) const noexcept {
    if (const auto it = Scope::find_type(name); it.has_value()) {
        return it;
    }
    return _builtin.find_type(name);
}

std::optional<FunctionVariable*> ModuleScope::find_method(
    Type* callee_type, const TypeList& argument_types, const std::string_view name) const noexcept {
    if (const auto it = Scope::find_method(callee_type, argument_types, name); it.has_value()) {
        return it;
    }
    return _builtin.find_method(callee_type, argument_types, name);
}

std::optional<Variable*> ModuleScope::find_variable(const std::string_view name) const noexcept {
    if (const auto found = Scope::find_variable(name); found.has_value()) {
        return found;
    }
    return _builtin.find_variable(name);
}

}  // namespace rain::lang::ast
