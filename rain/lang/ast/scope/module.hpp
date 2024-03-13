#pragma once

#include <tuple>

#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "rain/lang/ast/scope/builtin.hpp"
#include "rain/lang/ast/scope/scope.hpp"
#include "rain/lang/ast/type/function.hpp"
#include "rain/lang/ast/type/type.hpp"

namespace rain::lang::ast {

/**
 * The top-level scope for a module. A single module can loosely be described as everything defined
 * in a single source file.
 *
 * This scope is the parent of all other scopes in the module. It depends only on the single shared
 * immutable BuiltinScope that contains all builtin language features.
 */
class ModuleScope : public Scope {
    BuiltinScope& _builtin;

  public:
    ModuleScope(BuiltinScope& builtin_scope) : _builtin(builtin_scope) {}
    ~ModuleScope() override = default;

    [[nodiscard]] constexpr std::optional<Scope*> parent() const noexcept override {
        return &_builtin;
    }

    [[nodiscard]] constexpr const ModuleScope& module() const noexcept override { return *this; }
    [[nodiscard]] constexpr ModuleScope&       module() noexcept override { return *this; }

    [[nodiscard]] FunctionType* get_function_type(
        const TypeList& argument_types, std::optional<Type*> return_type) noexcept override;

    [[nodiscard]] std::optional<Type*> find_type(
        const std::string_view name) const noexcept override;

    [[nodiscard]] std::optional<FunctionVariable*> find_method(
        Type* callee_type, const TypeList& argument_types,
        const std::string_view name) const noexcept override;

    [[nodiscard]] std::optional<Variable*> find_variable(
        const std::string_view name) const noexcept override;
};

}  // namespace rain::lang::ast
