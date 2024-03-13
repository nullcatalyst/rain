#pragma once

#include <optional>
#include <tuple>

#include "absl/hash/hash.h"
#include "rain/lang/ast/type/function.hpp"
#include "rain/lang/ast/type/type.hpp"
#include "rain/lang/ast/var/variable.hpp"

namespace rain::lang::ast {

class ModuleScope;

/**
 *
 */
using TypeList = llvm::SmallVector<Type*, 4>;

/**
 * The key used to store and look up a function type in a scope.
 *
 * This is just a useful alias, since the type is long and can be prone to typos.
 */
using FunctionTypeKey = std::tuple<TypeList, Type*>;

class Scope {
  public:
    virtual ~Scope() = default;

    [[nodiscard]] virtual std::optional<Scope*>  parent() const noexcept = 0;
    [[nodiscard]] virtual const ModuleScope&     module() const noexcept = 0;
    [[nodiscard]] virtual constexpr ModuleScope& module() noexcept       = 0;

    [[nodiscard]] virtual FunctionTypePtr get_function_type(const TypeList& argument_types,
                                                            Type* return_type) noexcept = 0;

    [[nodiscard]] virtual std::optional<TypePtr> find_type(
        const std::string_view name) const noexcept = 0;

    [[nodiscard]] virtual std::optional<VariablePtr> find_method(
        const TypePtr& callee_type, const std::string_view name) const noexcept = 0;

    [[nodiscard]] virtual std::optional<VariablePtr> find_variable(
        const std::string_view name) const noexcept = 0;

    virtual void add_type(const std::string_view name, TypePtr type) noexcept = 0;

    virtual void add_method(const TypePtr& callee_type, const std::string_view name,
                            VariablePtr variable) noexcept = 0;

    virtual void add_variable(const std::string_view name, VariablePtr variable) noexcept = 0;
};

template <typename H>
H AbslHashValue(H h, const TypeList& type_list) {
    return h.combine_contiguous(std::move(h), type_list.data(), type_list.size());
}

template <typename H>
H AbslHashValue(H h, const FunctionTypeKey& function_type_key) {
    return H::combine(std::move(h), std::get<0>(function_type_key), std::get<1>(function_type_key));
}

}  // namespace rain::lang::ast
