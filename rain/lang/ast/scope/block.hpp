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

  public:
    BlockScope(Scope& parent) : _parent(parent), _module(parent.module()) {}
    ~BlockScope() override = default;

    [[nodiscard]] constexpr std::optional<Scope*> parent() const noexcept override {
        return &_parent;
    }

    [[nodiscard]] constexpr const ModuleScope& module() const noexcept override { return _module; }
    [[nodiscard]] constexpr ModuleScope&       module() noexcept override { return _module; }

    [[nodiscard]] FunctionType* get_function_type(
        const TypeList& argument_types, std::optional<Type*> return_type) noexcept override;

    [[nodiscard]] std::optional<Type*> find_type(
        const std::string_view name) const noexcept override;

    [[nodiscard]] std::optional<FunctionVariable*> find_method(
        Type* callee_type, const TypeList& argument_types,
        const std::string_view name) const noexcept override;

    [[nodiscard]] std::optional<Variable*> find_variable(
        const std::string_view name) const noexcept override;

    void add_type(const std::string_view name, std::unique_ptr<Type> type) noexcept override;

    void add_method(Type* callee_type, const std::string_view name,
                    std::unique_ptr<FunctionVariable> method) noexcept override;

    void add_variable(const std::string_view    name,
                      std::unique_ptr<Variable> variable) noexcept override;
};

}  // namespace rain::lang::ast
