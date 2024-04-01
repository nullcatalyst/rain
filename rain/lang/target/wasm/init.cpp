#include "rain/lang/target/wasm/init.hpp"

#include "llvm/ExecutionEngine/Interpreter.h"
#include "llvm/Support/TargetSelect.h"

namespace rain::lang::wasm {

void initialize_llvm() {
    static bool initialized = false;
    if (!initialized) {
        initialized = true;

        LLVMInitializeWebAssemblyTargetInfo();
        LLVMInitializeWebAssemblyTarget();
        LLVMInitializeWebAssemblyTargetMC();
        LLVMInitializeWebAssemblyAsmPrinter();
        LLVMInitializeWebAssemblyAsmParser();
        LLVMLinkInInterpreter();
    }
}

}  // namespace rain::lang::wasm
