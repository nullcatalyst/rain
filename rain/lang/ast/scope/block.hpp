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
    explicit BlockScope(Scope& parent);
    BlockScope(const BlockScope&)            = delete;
    BlockScope& operator=(const BlockScope&) = delete;
    explicit BlockScope(BlockScope&&);
    BlockScope& operator=(BlockScope&&) = delete;
    ~BlockScope() override              = default;

    [[nodiscard]] absl::Nullable<Scope*>       parent() const noexcept override { return &_parent; }
    [[nodiscard]] absl::Nonnull<ModuleScope*>  module() const noexcept override { return &_module; }
    [[nodiscard]] absl::Nonnull<BuiltinScope*> builtin() const noexcept override {
        return _module.builtin();
    }
};

}  // namespace rain::lang::ast
