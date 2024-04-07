#pragma once

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

  protected:
    using FunctionTypeKey =
        std::tuple<TypeList /*argument_types*/, absl::Nullable<Type*> /*return_type*/>;
    using FunctionVariableKey = std::tuple<absl::Nullable<Type*> /*callee*/, TypeList /*arguments*/,
                                           std::string_view /*name*/>;

    absl::flat_hash_map<std::string_view, absl::Nonnull<Type*>>         _named_types;
    absl::flat_hash_map<FunctionTypeKey, absl::Nonnull<FunctionType*>>  _function_types;
    absl::flat_hash_map<absl::Nonnull<Type*>, absl::Nonnull<MetaType*>> _meta_types;
    absl::flat_hash_set<std::unique_ptr<Type>>                          _owned_types;

    /**
     * Stores the set of types that need to be resolved after parsing.
     */
    std::vector<std::unique_ptr<UnresolvedType>> _unresolved_types;

    absl::flat_hash_map<FunctionVariableKey, absl::Nonnull<FunctionVariable*>> _function_variables;
    absl::flat_hash_map<std::string_view, absl::Nonnull<Variable*>>            _named_variables;
    absl::flat_hash_set<std::unique_ptr<Variable>>                             _owned_variables;

  public:
    virtual ~Scope() = default;

    [[nodiscard]] constexpr const absl::flat_hash_set<std::unique_ptr<Type>>& owned_types()
        const noexcept {
        return _owned_types;
    }
    [[nodiscard]] constexpr const absl::flat_hash_set<std::unique_ptr<Variable>>& owned_variables()
        const noexcept {
        return _owned_variables;
    }

    [[nodiscard]] virtual absl::Nullable<Scope*>       parent() const noexcept  = 0;
    [[nodiscard]] virtual absl::Nonnull<ModuleScope*>  module() const noexcept  = 0;
    [[nodiscard]] virtual absl::Nonnull<BuiltinScope*> builtin() const noexcept = 0;

    [[nodiscard]] constexpr absl::flat_hash_set<std::unique_ptr<Type>>& owned_types() noexcept {
        return _owned_types;
    }
    [[nodiscard]] constexpr absl::flat_hash_set<std::unique_ptr<Variable>>&
    owned_variables() noexcept {
        return _owned_variables;
    }

    [[nodiscard]] virtual absl::Nonnull<FunctionType*> get_function_type(
        const TypeList& argument_types, absl::Nullable<Type*> return_type) noexcept = 0;

    [[nodiscard]] virtual absl::Nullable<Type*> find_type(
        const std::string_view name) const noexcept;

    [[nodiscard]] virtual absl::Nonnull<Type*> find_or_unresolved_type(
        const std::string_view name, lex::Location location) noexcept;

    /**
     * The passed in `callee_type` can be null for any function that is not a method, and does not
     * need a callee object.
     */
    [[nodiscard]] virtual absl::Nullable<FunctionVariable*> find_function(
        absl::Nullable<Type*> callee_type, const TypeList& argument_types,
        const std::string_view name) const noexcept;

    [[nodiscard]] virtual absl::Nullable<Variable*> find_variable(
        const std::string_view name) const noexcept;

    virtual absl::Nonnull<Type*> add_type(const std::string_view name, std::unique_ptr<Type> type);

    virtual absl::Nonnull<FunctionVariable*> add_function(absl::Nullable<Type*>  callee_type,
                                                          const TypeList&        argument_types,
                                                          const std::string_view name,
                                                          std::unique_ptr<FunctionVariable> method);

    virtual absl::Nonnull<Variable*> add_variable(const std::string_view    name,
                                                  std::unique_ptr<Variable> variable);

    virtual util::Result<void> validate(Options& options);

  protected:
    [[nodiscard]] absl::Nullable<FunctionType*> _get_function_type(
        const TypeList& argument_types, absl::Nullable<Type*> return_type);

    absl::Nonnull<Type*> _add_owned_named_type(const std::string_view name,
                                               std::unique_ptr<Type>  type);

    absl::Nonnull<Type*>       _unwrap_type(absl::Nonnull<Type*> type);
    absl::Nonnull<const Type*> _unwrap_type(absl::Nonnull<const Type*> type) {
        return absl::Nonnull<const Type*>(_unwrap_type(absl::Nonnull<Type*>(type)));
    }
};

}  // namespace rain::lang::ast

#include "rain/lang/ast/type/function.hpp"
#include "rain/lang/ast/type/type.hpp"
#include "rain/lang/ast/type/unresolved.hpp"
#include "rain/lang/ast/var/function.hpp"
#include "rain/lang/ast/var/variable.hpp"
