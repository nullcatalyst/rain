#include "rain/code/initialize.hpp"

#include "llvm/ExecutionEngine/Interpreter.h"
#include "llvm/Support/TargetSelect.h"

namespace rain::code {

void initialize_llvm() {
    static bool initialized = false;
    if (!initialized) {
        initialized = true;
        // llvm::InitializeNativeTarget();
        // llvm::InitializeNativeTargetAsmPrinter();
        // llvm::InitializeNativeTargetAsmParser();

        // llvm::InitializeAllTargets();
        // llvm::InitializeAllAsmPrinters();
        // llvm::InitializeAllAsmParsers();

        LLVMInitializeWebAssemblyTargetInfo();
        LLVMInitializeWebAssemblyTarget();
        LLVMInitializeWebAssemblyTargetMC();
        LLVMInitializeWebAssemblyAsmPrinter();
        LLVMInitializeWebAssemblyAsmParser();
        LLVMLinkInInterpreter();
    }
}

}  // namespace rain::code
