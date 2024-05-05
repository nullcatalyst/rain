#pragma once

#include <array>

#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Value.h"
#include "rain/lang/code/context.hpp"

namespace rain::lang::code::util {

llvm::Value* create_optional_literal(Context& ctx, llvm::Type* llvm_optional_type, llvm::Value* llvm_value) {
    auto&        llvm_ir    = ctx.llvm_builder();

    llvm::Value* llvm_optional_value = llvm::PoisonValue::get(llvm_optional_type);
    if (llvm_value != nullptr) {
        llvm_optional_value = llvm_ir.CreateInsertValue(llvm_optional_value, llvm_value, 0);
    }
    return llvm_ir.CreateInsertValue(llvm_optional_value, llvm_ir.getInt1(llvm_value != nullptr), 1);
}

}  // namespace rain::lang::code::util
