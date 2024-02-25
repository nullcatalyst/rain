#include "rain/code/target.hpp"

#include "llvm/MC/TargetRegistry.h"
#include "llvm/Support/TargetSelect.h"
#include "rain/util/colors.hpp"
#include "rain/util/log.hpp"

namespace rain::code {

std::unique_ptr<llvm::TargetMachine> wasm_target_machine() {
    std::string error;

    std::string         target_triple = llvm::Triple::normalize("wasm32-unknown-unknown");
    const llvm::Target* target        = llvm::TargetRegistry::lookupTarget(target_triple, error);
    if (target == nullptr) {
        util::console_error(ANSI_RED, "failed to lookup target: ", ANSI_RESET, error);
        std::abort();
    }

    std::string cpu;
    std::string features =
        "+simd128,+sign-ext,+bulk-memory,+mutable-globals,+nontrapping-fptoint,+multivalue";
    return std::unique_ptr<llvm::TargetMachine>(
        target->createTargetMachine(target_triple, cpu, features, llvm::TargetOptions(),
                                    std::nullopt, std::nullopt, llvm::CodeGenOpt::Default));
}

}  // namespace rain::code