#pragma once

#include <memory>

#include "llvm/Target/TargetMachine.h"

namespace rain::lang::code::wasm {

void initialize_llvm();

std::unique_ptr<llvm::TargetMachine> target_machine();

}  // namespace rain::lang::code::wasm
