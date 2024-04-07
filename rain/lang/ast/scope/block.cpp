#include "rain/lang/ast/scope/block.hpp"

namespace rain::lang::ast {

// absl::Nonnull<FunctionType*> BlockScope::get_function_type(
//     const TypeList& argument_types, absl::Nullable<Type*> return_type) noexcept {
//     if (auto* type = Scope::_get_function_type_in_current_scope(argument_types, return_type);
//         type != nullptr) {
//         return type;
//     }

//     auto* parent = &_parent;
//     while (_parent != nullptr) {
//         if (auto* type = _parent->get_function_type_in_current_scope(argument_types,
//         return_type);
//             type != nullptr) {
//             return type;
//         }
//     }

//     return _create_function_type(argument_types, return_type);
// }

absl::Nullable<Type*> BlockScope::find_type(const std::string_view name) const noexcept {
    if (const auto it = Scope::find_type(name); it != nullptr) {
        return it;
    }
    return _parent.find_type(name);
}

absl::Nullable<FunctionVariable*> BlockScope::find_function(
    absl::Nullable<Type*> callee_type, const TypeList& argument_types,
    const std::string_view name) const noexcept {
    if (const auto it = Scope::find_function(callee_type, argument_types, name); it != nullptr) {
        return it;
    }
    return _parent.find_function(callee_type, argument_types, name);
}

absl::Nullable<Variable*> BlockScope::find_variable(const std::string_view name) const noexcept {
    if (const auto found = Scope::find_variable(name); found != nullptr) {
        return found;
    }
    return _parent.find_variable(name);
}

}  // namespace rain::lang::ast
