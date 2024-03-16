#pragma once

#include <string_view>

#include "llvm/ADT/SmallVector.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Value.h"
#include "rain/lang/ast/var/variable.hpp"

namespace rain::lang::ast {

class FunctionVariable : public Variable {
    std::string_view _name;
    FunctionType*    _function_type;

  public:
    FunctionVariable(std::string_view name, FunctionType* type)
        : _name(name), _function_type(type) {}
    ~FunctionVariable() override = default;

    [[nodiscard]] std::string_view name() const noexcept override { return _name; }
    [[nodiscard]] Type*            type() const noexcept override { return _function_type; }

    [[nodiscard]] virtual llvm::Value* build_call(
        llvm::IRBuilder<>&                        builder,
        const llvm::SmallVector<llvm::Value*, 4>& arguments) const noexcept {
        return nullptr;
    }
};

}  // namespace rain::lang::ast
