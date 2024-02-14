#pragma once

#include <memory>

#include "llvm/Target/TargetMachine.h"

namespace cirrus::code {

std::unique_ptr<llvm::TargetMachine> wasm_target_machine();

}  // namespace cirrus::code
