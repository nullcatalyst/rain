#include "rain/lang/ast/scope/scope.hpp"

#include "rain/lang/ast/scope/builtin.hpp"
#include "rain/lang/ast/type/function.hpp"
#include "rain/lang/ast/type/meta.hpp"
#include "rain/lang/ast/type/type.hpp"
#include "rain/lang/ast/var/block.hpp"
#include "rain/lang/ast/var/function.hpp"
#include "rain/lang/ast/var/variable.hpp"
#include "rain/lang/err/syntax.hpp"

namespace rain::lang::ast {

absl::Nullable<FunctionType*> Scope::_get_function_type_in_current_scope(
    const TypeList& argument_types, Type* return_type) {
    const bool owns_types =
        (argument_types.empty() && builtin() == this) ||
        std::any_of(argument_types.begin(), argument_types.end(),
                    [this](auto* type) { return _owned_types.contains(_unwrap_type(type)); }) ||
        (return_type != nullptr && _owned_types.contains(_unwrap_type(return_type)));
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

    return _create_function_type(argument_types, return_type);
}

absl::Nonnull<FunctionType*> Scope::_create_function_type(const TypeList& argument_types,
                                                          Type*           return_type) {
    auto  function_type     = std::make_unique<FunctionType>(argument_types, return_type, false);
    auto* function_type_ptr = function_type.get();
    _function_types.insert_or_assign(std::make_tuple(argument_types, return_type),
                                     function_type_ptr);
    _owned_types.insert(std::move(function_type));
    return function_type_ptr;
}

absl::Nonnull<Type*> Scope::_add_owned_named_type(const std::string_view name,
                                                  std::unique_ptr<Type>  type) {
    assert(type != nullptr);

    auto* type_ptr = type.get();
    _named_types.insert_or_assign(name, type_ptr);
    _owned_types.insert(std::move(type));

    if (auto* scope = builtin(); scope != nullptr) {
        auto meta_type = std::make_unique<MetaType>(type_ptr);

        auto variable = std::make_unique<BlockVariable>(name, meta_type.get(), false);
        _named_variables.insert_or_assign(name, variable.get());
        _owned_variables.insert(std::move(variable));

        _meta_types.insert_or_assign(type_ptr, meta_type.get());
        _owned_types.insert(std::move(meta_type));
    }

    return type_ptr;
}

absl::Nonnull<Type*> Scope::_unwrap_type(absl::Nonnull<Type*> type) {
    for (;;) {
        switch (type->kind()) {
            case serial::TypeKind::Array:
                type = &static_cast<ArrayType&>(*type).type();
                break;

            case serial::TypeKind::Optional:
                type = &static_cast<OptionalType&>(*type).type();
                break;

            default:
                return type;
        }
    }
}

absl::Nonnull<FunctionType*> Scope::get_function_type(const TypeList&       argument_types,
                                                      absl::Nullable<Type*> return_type) noexcept {
    if (auto* type = Scope::_get_function_type_in_current_scope(argument_types, return_type);
        type != nullptr) {
        return type;
    }

    auto* parent = this->parent();
    while (parent != nullptr) {
        if (auto* type = parent->_get_function_type_in_current_scope(argument_types, return_type);
            type != nullptr) {
            return type;
        }
        parent = parent->parent();
    }

    // return _create_function_type(argument_types, return_type);

    auto  function_type     = std::make_unique<FunctionType>(argument_types, return_type, true);
    auto* function_type_ptr = function_type.get();
    _function_types.insert_or_assign(std::make_tuple(argument_types, return_type),
                                     function_type_ptr);
    _owned_types.insert(std::move(function_type));
    return function_type_ptr;
}

absl::Nullable<Type*> Scope::find_type(const std::string_view name) const noexcept {
    if (const auto it = _named_types.find(name); it != _named_types.end()) {
        return it->second;
    }

    return nullptr;
}

absl::Nonnull<Type*> Scope::find_or_unresolved_type(const std::string_view name,
                                                    lex::Location          location) noexcept {
    // NOTE: We cannot use the `find_type()` method here, because some derived classes may
    // recursively search parent scopes for a type. This would be a problem if a tyep defined in
    // this scope shadows the type defined in a parent scope, and we accidentally pass back the
    // parent's version of the type instead of our own (not yet defined) version.

    if (const auto it = _named_types.find(name); it != _named_types.end()) {
        return it->second;
    }

    return _unresolved_types.emplace_back(std::make_unique<UnresolvedType>(name, location)).get();
}

absl::Nullable<FunctionVariable*> Scope::find_function(absl::Nullable<Type*>  callee_type,
                                                       const TypeList&        argument_types,
                                                       const std::string_view name) const noexcept {
    if (const auto it =
            _function_variables.find(std::make_tuple(callee_type, argument_types, name));
        it != _function_variables.end()) {
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

absl::Nonnull<Type*> Scope::add_type(const std::string_view name, std::unique_ptr<Type> type) {
    return _add_owned_named_type(name, std::move(type));
}

absl::Nonnull<FunctionVariable*> Scope::add_function(absl::Nullable<Type*>  callee_type,
                                                     const TypeList&        argument_types,
                                                     const std::string_view name,
                                                     std::unique_ptr<FunctionVariable> function) {
    assert(function != nullptr);

    auto* function_ptr = function.get();
    _function_variables.insert_or_assign(std::make_tuple(callee_type, argument_types, name),
                                         function_ptr);
    _owned_variables.insert(std::move(function));
    return function_ptr;
}

absl::Nonnull<Variable*> Scope::add_variable(const std::string_view    name,
                                             std::unique_ptr<Variable> variable) {
    assert(variable != nullptr);

    auto* variable_ptr = variable.get();
    _named_variables.insert_or_assign(name, variable_ptr);
    _owned_variables.insert(std::move(variable));
    return variable_ptr;
}

util::Result<void> Scope::validate(Options& options) {
    for (auto& unresolved_type : _unresolved_types) {
        if (!unresolved_type->is_used()) {
            // Unresolved types that are not referenced anywhere can occur because of attempting to
            // parse a method callee type, but it isn't a method, which causes the parsing to fail
            // and fallback to parsing a simple function, leaving the callee type unreferenced.
            continue;
        }

        auto  name = unresolved_type->name();
        auto* type = find_type(name);
        if (type == nullptr) {
            return ERR_PTR(err::SyntaxError, unresolved_type->location(),
                           absl::StrCat("no type named '", name, "' found in scope"));
        }

        unresolved_type->resolve_to_type(type);
    }

    return {};
}

util::Result<void> Scope::cleanup() {
    _unresolved_types.clear();

    // If a type should have been replaced with another type, then that means the original tyep is
    // no longer needed. We can remove it from the list of owned types.
    for (auto it = _owned_types.begin(); it != _owned_types.end(); ++it) {
        auto* owned_type   = it->get();
        auto* replace_with = owned_type->should_be_replaced_with(*this);

        if (replace_with != owned_type) {
            _owned_types.erase(it);
        }
    }

    return {};
}

}  // namespace rain::lang::ast
