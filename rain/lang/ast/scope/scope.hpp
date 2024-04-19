#pragma once

#include <memory>
#include <tuple>
#include <vector>

#include "absl/base/nullability.h"
#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/hash/hash.h"
#include "llvm/ADT/SmallVector.h"
#include "rain/lang/lex/location.hpp"
#include "rain/lang/options.hpp"
#include "rain/util/result.hpp"

namespace llvm {

template <typename H, typename T>
inline H AbslHashValue(H h, const llvm::SmallVector<T*, 4>& vec) {
    return H::combine_contiguous(std::move(h), vec.data(), vec.size());
}

}  // namespace llvm

namespace rain::lang::ast {

class Type;
class MetaType;
class FunctionType;
class UnresolvedType;

class Variable;
class FunctionVariable;

class ModuleScope;
class BuiltinScope;

class Scope {
  public:
    using TypeList = llvm::SmallVector<Type*, 4>;

    using FunctionTypeKey =
        std::tuple<absl::Nullable<Type*> /*callee_type*/, TypeList /*argument_types*/,
                   absl::Nullable<Type*> /*return_type*/>;

    /**
     * Since we do not know the return type of a function until after we have already found the
     * correct overload for that function, we cannot use the full function type as the lookup key.
     * Instead we have to look up based on the function name, the callee type, and the argument
     * types.
     */
    using FunctionVariableKey =
        std::tuple<std::string_view /*name*/, absl::Nullable<Type*> /*callee_type*/,
                   TypeList /*argument_types*/>;

  protected:
    absl::flat_hash_map<std::string_view, absl::Nonnull<Type*>>         _named_types;
    absl::flat_hash_map<FunctionTypeKey, absl::Nonnull<FunctionType*>>  _function_types;
    absl::flat_hash_map<absl::Nonnull<Type*>, absl::Nonnull<MetaType*>> _meta_types;
    absl::flat_hash_set<std::unique_ptr<Type>>                          _owned_types;

    absl::flat_hash_map<FunctionVariableKey, absl::Nonnull<FunctionVariable*>> _function_variables;
    absl::flat_hash_map<std::string_view, absl::Nonnull<Variable*>>            _named_variables;
    absl::flat_hash_set<std::unique_ptr<Variable>>                             _owned_variables;

    /**
     * Stores the set of types that need to be resolved after parsing.
     *
     * These are not stored in either the `_named_types` map or the `_owned_types` set because they
     * are inherently short lived, and only exist between the time that the source code is parsed
     * and the time that the AST is validated.
     */
    std::vector<std::unique_ptr<Type>> _unresolved_types;

    /**
     * Stores the set of functions that need to be resolved after parsing.
     *
     * These cannot be stored in the _function_variables map because those are keyed off of the
     * function type, and the function type is not known until after all the types are resolved.
     */
    std::vector<std::unique_ptr<FunctionVariable>> _unresolved_functions;

  public:
    Scope()                        = default;
    Scope(const Scope&)            = delete;
    Scope& operator=(const Scope&) = delete;
    explicit Scope(Scope&&);
    Scope& operator=(Scope&&) = delete;
    virtual ~Scope()          = default;

    [[nodiscard]] constexpr const auto& owned_types() const noexcept { return _owned_types; }
    [[nodiscard]] constexpr auto&       owned_types() noexcept { return _owned_types; }
    [[nodiscard]] constexpr const auto& owned_variables() const noexcept {
        return _owned_variables;
    }
    [[nodiscard]] constexpr auto& owned_variables() noexcept { return _owned_variables; }

    [[nodiscard]] virtual absl::Nullable<Scope*>       parent() const noexcept  = 0;
    [[nodiscard]] virtual absl::Nonnull<ModuleScope*>  module() const noexcept  = 0;
    [[nodiscard]] virtual absl::Nonnull<BuiltinScope*> builtin() const noexcept = 0;

    ////////////////////////////////////////////////////////////////
    // Find AST nodes

    [[nodiscard]] absl::Nonnull<Type*> find_or_create_unresolved_named_type(
        const std::string_view name, lex::Location location) noexcept;
    [[nodiscard]] absl::Nullable<Type*> find_named_type(const std::string_view name) const noexcept;

    [[nodiscard]] absl::Nonnull<FunctionType*> find_or_create_unresolved_function_type(
        absl::Nullable<Type*> callee_type, const TypeList& argument_types,
        absl::Nullable<Type*> return_type) noexcept;
    [[nodiscard]] absl::Nonnull<FunctionType*> get_resolved_function_type(
        absl::Nullable<Type*> callee_type, const TypeList& argument_types,
        absl::Nullable<Type*> return_type) noexcept;

    /**
     * The passed in `callee_type` can be null for any function that is not a method, and does not
     * need a callee object.
     */
    [[nodiscard]] absl::Nullable<FunctionVariable*> find_function(
        const std::string_view name, absl::Nullable<Type*> callee_type,
        TypeList argument_types) const noexcept;

    [[nodiscard]] absl::Nullable<Variable*> find_variable(
        const std::string_view name) const noexcept;

    template <typename Fn>
    void for_each_function(Fn&& fn) const {
        for (const auto& [_, function] : _function_variables) {
            fn(*function);
        }
    }

    ////////////////////////////////////////////////////////////////
    // Add AST nodes

    virtual absl::Nonnull<Type*> add_named_type(const std::string_view name,
                                                std::unique_ptr<Type>  type) noexcept;

    virtual absl::Nonnull<FunctionVariable*> create_unresolved_function(
        const std::string_view name, absl::Nullable<FunctionType*> function_type,
        lex::Location location) noexcept;

    virtual absl::Nonnull<Variable*> add_variable(std::unique_ptr<Variable> variable) noexcept;
    virtual absl::Nonnull<FunctionVariable*> add_resolved_function(
        std::unique_ptr<FunctionVariable> function_variable) noexcept;

    ////////////////////////////////////////////////////////////////
    // Validation

    virtual util::Result<void> validate(Options& options);
    virtual util::Result<void> cleanup();
};

}  // namespace rain::lang::ast

#include "rain/lang/ast/type/function.hpp"
#include "rain/lang/ast/type/type.hpp"
#include "rain/lang/ast/type/unresolved.hpp"
#include "rain/lang/ast/var/function.hpp"
#include "rain/lang/ast/var/variable.hpp"
