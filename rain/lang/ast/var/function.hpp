#pragma once

#include <string_view>

#include "llvm/ADT/SmallVector.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Value.h"
#include "rain/lang/ast/type/function.hpp"
#include "rain/lang/ast/var/variable.hpp"
#include "rain/util/maybe_owned_ptr.hpp"

namespace rain::lang::ast {

class FunctionVariable : public Variable {
    std::string_view             _name;
    absl::Nonnull<FunctionType*> _function_type;

  public:
    FunctionVariable(const std::string_view name, absl::Nonnull<FunctionType*> type)
        : _name(name), _function_type(type) {}
    ~FunctionVariable() override = default;

    [[nodiscard]] std::string_view     name() const noexcept override { return _name; }
    [[nodiscard]] absl::Nonnull<Type*> type() const noexcept override { return _function_type; }
    [[nodiscard]] bool                 mutable_() const noexcept override { return false; }
    [[nodiscard]] absl::Nonnull<FunctionType*> function_type() const noexcept {
        return _function_type;
    }

    [[nodiscard]] virtual llvm::Value* build_call(
        llvm::IRBuilder<>&                        builder,
        const llvm::SmallVector<llvm::Value*, 4>& arguments) const noexcept {
        return nullptr;
    }

    [[nodiscard]] util::Result<void> validate(Scope& scope) noexcept override;
};

}  // namespace rain::lang::ast
