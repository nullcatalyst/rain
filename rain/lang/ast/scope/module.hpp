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
    ModuleScope(BuiltinScope& builtin) : _builtin(builtin) {}
    ~ModuleScope() override = default;

    [[nodiscard]] absl::Nullable<Scope*>      parent() const noexcept override { return &_builtin; }
    [[nodiscard]] absl::Nonnull<ModuleScope*> module() const noexcept override {
        return const_cast<ModuleScope*>(this);
    }
    [[nodiscard]] absl::Nonnull<BuiltinScope*> builtin() const noexcept override {
        return &_builtin;
    }
};

}  // namespace rain::lang::ast
