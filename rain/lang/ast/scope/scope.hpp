#pragma once

#include <tuple>

#include "absl/base/nullability.h"
#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "absl/hash/hash.h"
#include "llvm/ADT/SmallVector.h"

namespace rain::lang::ast {

class Type;
class FunctionType;

class Variable;
class FunctionVariable;

class ModuleScope;
class BuiltinScope;

class Scope {
  public:
    class TypeList : public llvm::SmallVector<absl::Nonnull<Type*>, 4> {
      public:
        using llvm::SmallVector<Type*, 4>::SmallVector;

        template <typename H>
        friend H AbslHashValue(H h, const TypeList& type_list) {
            return h.combine_contiguous(std::move(h), type_list.data(), type_list.size());
        }
    };

  protected:
    using FunctionTypeKey =
        std::tuple<TypeList /*argument_types*/, absl::Nullable<Type*> /*return_type*/>;
    using MethodVariableKey = std::tuple<absl::Nonnull<Type*> /*callee*/, TypeList /*arguments*/,
                                         std::string_view /*name*/>;

    absl::flat_hash_map<std::string_view, absl::Nonnull<Type*>>        _named_types;
    absl::flat_hash_map<FunctionTypeKey, absl::Nonnull<FunctionType*>> _function_types;
    absl::flat_hash_set<std::unique_ptr<Type>>                         _owned_types;

    absl::flat_hash_map<MethodVariableKey, absl::Nonnull<FunctionVariable*>> _method_variables;
    absl::flat_hash_map<std::string_view, absl::Nonnull<Variable*>>          _named_variables;
    absl::flat_hash_set<std::unique_ptr<Variable>>                           _owned_variables;

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

    [[nodiscard]] virtual absl::Nonnull<FunctionType*> get_function_type(
        const TypeList& argument_types, absl::Nullable<Type*> return_type) noexcept = 0;

    [[nodiscard]] virtual absl::Nullable<Type*> find_type(
        const std::string_view name) const noexcept;

    [[nodiscard]] virtual absl::Nullable<FunctionVariable*> find_method(
        absl::Nonnull<Type*> callee_type, const TypeList& argument_types,
        const std::string_view name) const noexcept;

    [[nodiscard]] virtual absl::Nullable<Variable*> find_variable(
        const std::string_view name) const noexcept;

    virtual absl::Nonnull<Type*> add_type(const std::string_view name,
                                          std::unique_ptr<Type>  type) noexcept;

    virtual absl::Nonnull<FunctionVariable*> add_method(
        Type* callee_type, const TypeList& argument_types, const std::string_view name,
        std::unique_ptr<FunctionVariable> method) noexcept;

    virtual absl::Nonnull<Variable*> add_variable(const std::string_view    name,
                                                  std::unique_ptr<Variable> variable) noexcept;

  protected:
    [[nodiscard]] absl::Nullable<FunctionType*> _get_function_type(
        const TypeList& argument_types, absl::Nullable<Type*> return_type) noexcept;
};

}  // namespace rain::lang::ast

#include "rain/lang/ast/type/function.hpp"
#include "rain/lang/ast/type/type.hpp"
#include "rain/lang/ast/var/function.hpp"
#include "rain/lang/ast/var/variable.hpp"
