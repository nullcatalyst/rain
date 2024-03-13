#pragma once

#include <optional>
#include <tuple>

#include "absl/hash/hash.h"
#include "rain/lang/ast/type/function.hpp"
#include "rain/lang/ast/type/type.hpp"
#include "rain/lang/ast/var/function.hpp"
#include "rain/lang/ast/var/variable.hpp"

namespace rain::lang::ast {

class ModuleScope;

class TypeList : public llvm::SmallVector<Type*, 4> {
  public:
    using llvm::SmallVector<Type*, 4>::SmallVector;

    template <typename H>
    friend H AbslHashValue(H h, const TypeList& type_list) {
        return h.combine_contiguous(std::move(h), type_list.data(), type_list.size());
    }
};

using FunctionTypeKey = std::tuple<TypeList, Type*>;

class Scope {
  protected:
    absl::flat_hash_map<std::string_view, Type*>        _named_types;
    absl::flat_hash_map<FunctionTypeKey, FunctionType*> _function_types;
    absl::flat_hash_set<std::unique_ptr<Type>>          _owned_types;

    absl::flat_hash_map<
        std::tuple<Type* /*callee*/, TypeList /*arguments*/, std::string_view /*name*/>,
        FunctionVariable*>
                                                     _methods;
    absl::flat_hash_map<std::string_view, Variable*> _named_variables;
    absl::flat_hash_set<std::unique_ptr<Variable>>   _owned_variables;

  public:
    virtual ~Scope() = default;

    [[nodiscard]] virtual std::optional<Scope*>  parent() const noexcept = 0;
    [[nodiscard]] virtual const ModuleScope&     module() const noexcept = 0;
    [[nodiscard]] virtual constexpr ModuleScope& module() noexcept       = 0;

    [[nodiscard]] virtual FunctionType* get_function_type(
        const TypeList& argument_types, std::optional<Type*> return_type) noexcept;

    [[nodiscard]] virtual std::optional<Type*> find_type(
        const std::string_view name) const noexcept;

    [[nodiscard]] virtual std::optional<FunctionVariable*> find_method(
        Type* callee_type, const TypeList& argument_types,
        const std::string_view name) const noexcept;

    [[nodiscard]] virtual std::optional<Variable*> find_variable(
        const std::string_view name) const noexcept;

    virtual void add_type(const std::string_view name, std::unique_ptr<Type> type) noexcept;

    virtual void add_method(Type* callee_type, const std::string_view name,
                            std::unique_ptr<FunctionVariable> method) noexcept;

    virtual void add_variable(const std::string_view    name,
                              std::unique_ptr<Variable> variable) noexcept;
};

}  // namespace rain::lang::ast
