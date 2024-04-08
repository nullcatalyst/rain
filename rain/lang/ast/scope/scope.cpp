#include "rain/lang/ast/scope/scope.hpp"

#include "rain/lang/ast/scope/builtin.hpp"
#include "rain/lang/ast/type/function.hpp"
#include "rain/lang/ast/type/meta.hpp"
#include "rain/lang/ast/type/type.hpp"
#include "rain/lang/ast/type/unresolved.hpp"
#include "rain/lang/ast/var/block.hpp"
#include "rain/lang/ast/var/function.hpp"
#include "rain/lang/ast/var/variable.hpp"
#include "rain/lang/err/syntax.hpp"

namespace rain::lang::ast {

namespace {

absl::Nonnull<const Type*> _unwrap_type(absl::Nonnull<const Type*> type) {
    for (;;) {
        switch (type->kind()) {
            case serial::TypeKind::Array:
                type = &static_cast<const ArrayType&>(*type).type();
                break;

            case serial::TypeKind::Optional:
                type = &static_cast<const OptionalType&>(*type).type();
                break;

            default:
                return type;
        }
    }
}

absl::Nonnull<Type*> _unwrap_type(absl::Nonnull<Type*> type) {
    return const_cast<absl::Nonnull<Type*>>(
        _unwrap_type(const_cast<absl::Nonnull<const Type*>>(type)));
}

}  // namespace

////////////////////////////////////////////////////////////////
// Find AST nodes

absl::Nonnull<Type*> Scope::find_or_create_unresolved_named_type(const std::string_view name,
                                                                 lex::Location location) noexcept {
    // NOTE: We cannot use the `find_named_type()` method here, because some derived classes may
    // recursively search parent scopes for a type. This would be a problem if a tyep defined in
    // this scope shadows the type defined in a parent scope, and we accidentally pass back the
    // parent's version of the type instead of our own (not yet defined) version.

    if (const auto it = _named_types.find(name); it != _named_types.end()) {
        return it->second;
    }

    return _unresolved_types.emplace_back(std::make_unique<UnresolvedType>(name, location)).get();
}

absl::Nullable<Type*> Scope::find_named_type(const std::string_view name) const noexcept {
    const Scope* scope = this;
    do {
        // Search recursively through the scope and its parents for the named type.
        if (const auto it = scope->_named_types.find(name); it != scope->_named_types.end()) {
            return it->second;
        }

        scope = scope->parent();
    } while (scope != nullptr);

    return nullptr;
}

absl::Nonnull<FunctionType*> Scope::find_or_create_unresolved_function_type(
    const TypeList& argument_types, absl::Nullable<Type*> return_type) noexcept {
    // This CANNOT be a recursive search through the parent scopes, because one or more of the types
    // in the function may not be fully resolved. And without the types being resolved, we don't
    // know which scope they are defined in, and type shadowing across scopes would change the
    // resolved type.
    auto key = std::make_tuple(argument_types, return_type);
    if (const auto it = _function_types.find(key); it != _function_types.end()) {
        return it->second;
    }

    // Create an unresolved version of the function type. We couldn't tell which scope the type
    // should properly linve in, so just create it in the current scope and delete it later
    // after the real type has been resolved.
    auto  function_type     = std::make_unique<FunctionType>(argument_types, return_type);
    auto* function_type_ptr = function_type.get();
    _unresolved_types.emplace_back(std::move(function_type));
    return function_type_ptr;
}

absl::Nonnull<FunctionType*> Scope::get_function_type(const TypeList&       argument_types,
                                                      absl::Nullable<Type*> return_type) noexcept {
    // Unwrap all of the types that we need to check against once, instead of repeatedl unwrapping
    // on every check against each scope in the hierarchy.
    TypeList unwrapped_argument_types;
    unwrapped_argument_types.reserve(argument_types.size());
    for (auto* type : argument_types) {
        unwrapped_argument_types.emplace_back(_unwrap_type(type));
    }
    absl::Nullable<Type*> unwrapped_return_type =
        return_type != nullptr ? _unwrap_type(return_type) : nullptr;

    Scope* scope = this;
    do {
        auto key = std::make_tuple(argument_types, return_type);
        if (const auto it = scope->_function_types.find(key); it != scope->_function_types.end()) {
            return it->second;
        }

        const bool owns_any_type =
            // Check return type
            (unwrapped_return_type != nullptr &&
             scope->_owned_types.contains(unwrapped_return_type)) ||
            // Check argument types
            std::any_of(unwrapped_argument_types.begin(), unwrapped_argument_types.end(),
                        [scope](auto* type) { return scope->_owned_types.contains(type); });
        if (owns_any_type) {
            // If one of the types is owned by the current scope, then this is the highest scope
            // that makes sense to own the function type.

            auto  function_type     = std::make_unique<FunctionType>(argument_types, return_type);
            auto* function_type_ptr = function_type.get();
            scope->_function_types.insert_or_assign(key, function_type_ptr);
            scope->_owned_types.insert(std::move(function_type));
            return function_type_ptr;
        }

        scope = scope->parent();
    } while (scope != nullptr);

    util::panic("no scope found to own the function type; this is an internal error");
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

////////////////////////////////////////////////////////////////
// Add AST nodes

absl::Nonnull<Type*> Scope::add_named_type(const std::string_view name,
                                           std::unique_ptr<Type>  type) {
    assert(type != nullptr);
    assert(type->kind() != serial::TypeKind::Unresolved);

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

////////////////////////////////////////////////////////////////
// Validation

util::Result<void> Scope::validate(Options& options) { return {}; }

util::Result<void> Scope::cleanup() {
    // _unresolved_types.clear();

    // Performing a swap here allows the capacity of the vector to be freed, instead of `.clear()`
    // just setting a "used size" to 0.
    decltype(_unresolved_types) empty_unresolved_types;
    std::swap(_unresolved_types, empty_unresolved_types);

    return {};
}

}  // namespace rain::lang::ast
