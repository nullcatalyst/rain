#include "rain/lang/ast/scope/module.hpp"

namespace rain::lang::ast {

absl::Nullable<Type*> ModuleScope::find_type(const std::string_view name) const noexcept {
    if (const auto it = Scope::find_type(name); it != nullptr) {
        return it;
    }
    return _builtin.find_type(name);
}

absl::Nullable<FunctionVariable*> ModuleScope::find_function(
    absl::Nullable<Type*> callee_type, const TypeList& argument_types,
    const std::string_view name) const noexcept {
    if (const auto it = Scope::find_function(callee_type, argument_types, name); it != nullptr) {
        return it;
    }
    return _builtin.find_function(callee_type, argument_types, name);
}

absl::Nullable<Variable*> ModuleScope::find_variable(const std::string_view name) const noexcept {
    if (const auto found = Scope::find_variable(name); found != nullptr) {
        return found;
    }
    return _builtin.find_variable(name);
}

}  // namespace rain::lang::ast
