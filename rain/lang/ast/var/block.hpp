#pragma once

#include <string_view>

#include "llvm/ADT/SmallVector.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Value.h"

namespace rain::lang::ast {

class BlockVariable : public Variable {
    std::string_view _name;
    Type*            _type;

  public:
    FunctionVariable(std::string_view name, Type* type) : _name(name), _type(type) {}
    ~FunctionVariable() override = default;

    [[nodiscard]] std::string_view name() const noexcept override { return _name; }
    [[nodiscard]] Type*            type() const noexcept override { return _type; }
};

}  // namespace rain::lang::ast
