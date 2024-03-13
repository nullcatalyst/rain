#pragma once

#include <string_view>

#include "llvm/ADT/SmallVector.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Value.h"

namespace rain::lang::ast {

class FunctionVariable : public Variable {
    std::string_view _name;
    FunctionTypePtr  _function_type;

  public:
    FunctionVariable(std::string_view name, FunctionTypePtr type)
        : _name(name), _function_type(std::move(type)) {}
    ~FunctionVariable() override = default;

    [[nodiscard]] std::string_view name() const noexcept override { return "builtin"; }
    [[nodiscard]] TypePtr          type() const noexcept override { return _function_type; }
    [[nodiscard]] virtual bool     is_method() const noexcept { return false; }

    [[nodiscard]] virtual const llvm::Value* build_call(
        llvm::IRBuilder<>&                        builder,
        const llvm::SmallVector<llvm::Value*, 4>& arguments) const noexcept;
};

}  // namespace rain::lang::ast
