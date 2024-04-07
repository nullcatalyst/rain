#pragma once

#include <tuple>

#include "absl/base/nullability.h"
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
    BlockScope(Scope& parent) : _parent(parent), _module(*parent.module()) {}
    ~BlockScope() override = default;

    [[nodiscard]] absl::Nullable<Scope*>       parent() const noexcept override { return &_parent; }
    [[nodiscard]] absl::Nonnull<ModuleScope*>  module() const noexcept override { return &_module; }
    [[nodiscard]] absl::Nonnull<BuiltinScope*> builtin() const noexcept override {
        return _module.builtin();
    }

    // [[nodiscard]] absl::Nonnull<FunctionType*> get_function_type(
    //     const TypeList& argument_types, absl::Nullable<Type*> return_type) noexcept override;

    [[nodiscard]] absl::Nullable<Type*> find_type(
        const std::string_view name) const noexcept override;

    [[nodiscard]] absl::Nullable<FunctionVariable*> find_function(
        absl::Nullable<Type*> callee_type, const TypeList& argument_types,
        const std::string_view name) const noexcept override;

    [[nodiscard]] absl::Nullable<Variable*> find_variable(
        const std::string_view name) const noexcept override;
};

}  // namespace rain::lang::ast
