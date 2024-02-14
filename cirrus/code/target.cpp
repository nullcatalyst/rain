#include "cirrus/code/target.hpp"

#include <iostream>

#include "llvm/MC/TargetRegistry.h"
#include "llvm/Support/TargetSelect.h"

namespace cirrus::code {

std::unique_ptr<llvm::TargetMachine> wasm_target_machine() {
    std::string error;

    std::string         target_triple = llvm::Triple::normalize("wasm32-unknown-unknown");
    const llvm::Target* target        = llvm::TargetRegistry::lookupTarget(target_triple, error);
    if (target == nullptr) {
        std::cerr << "failed to lookup target: " << error << '\n';
        std::abort();
    }

    std::string cpu;
    std::string features =
        "+simd128,+sign-ext,+bulk-memory,+mutable-globals,+nontrapping-fptoint,+multivalue";
    return std::unique_ptr<llvm::TargetMachine>(
        target->createTargetMachine(target_triple, cpu, features, llvm::TargetOptions(),
                                    std::nullopt, std::nullopt, llvm::CodeGenOptLevel::Default));
}

}  // namespace cirrus::code