#pragma once

#include <tuple>

#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "rain/lang/ast/scope/module.hpp"
#include "rain/lang/ast/scope/scope.hpp"
#include "rain/lang/ast/type/type.hpp"

namespace rain::lang::ast {

class BlockScope : public Scope {
    Scope&       _parent;
    ModuleScope& _module;

    absl::flat_hash_map<std::string_view, TypePtr>                        _named_types;
    absl::flat_hash_map<FunctionTypeKey, FunctionTypePtr>                 _function_types;
    absl::flat_hash_map<std::tuple<Type*, std::string_view>, VariablePtr> _methods;
    absl::flat_hash_map<std::string_view, VariablePtr>                    _variables;

    absl::flat_hash_set<TypePtr> _owned_types;

  public:
    BlockScope(Scope& parent) : _parent(parent), _module(parent.module()) {}
    ~BlockScope() override = default;

    [[nodiscard]] constexpr std::optional<Scope*> parent() const noexcept override {
        return &_parent;
    }

    [[nodiscard]] constexpr const ModuleScope& module() const noexcept override { return _module; }
    [[nodiscard]] constexpr ModuleScope&       module() noexcept override { return _module; }

    [[nodiscard]] FunctionTypePtr get_function_type(const TypeList& argument_types,
                                                    Type*           return_type) noexcept override;

    [[nodiscard]] std::optional<TypePtr> find_type(
        const std::string_view name) const noexcept override;

    [[nodiscard]] std::optional<VariablePtr> find_method(
        const TypePtr& callee_type, const std::string_view name) const noexcept override;

    [[nodiscard]] std::optional<VariablePtr> find_variable(
        const std::string_view name) const noexcept override;

    void add_type(const std::string_view name, TypePtr type) noexcept override;

    void add_method(const TypePtr& callee_type, const std::string_view name,
                    VariablePtr variable) noexcept override;

    void add_variable(const std::string_view name, VariablePtr variable) noexcept override;
};

}  // namespace rain::lang::ast
