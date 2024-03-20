#pragma once

#include <memory>

#include "llvm/Target/TargetMachine.h"

#if defined(RAIN_TARGET_WASM)
#include "rain/lang/code/target/wasm/target.hpp"
#endif  // defined(RAIN_TARGET_WASM)

namespace rain::lang::code {

inline void initialize_llvm() {
#if defined(RAIN_TARGET_WASM)
    wasm::initialize_llvm();
#else
#error "Unsupported target"
#endif  // defined(RAIN_TARGET_WASM)
}

inline std::unique_ptr<llvm::TargetMachine> target_machine() {
#if defined(RAIN_TARGET_WASM)
    return wasm::target_machine();
#else
#error "Unsupported target"
#endif  // defined(RAIN_TARGET_WASM)
}

}  // namespace rain::lang::code
