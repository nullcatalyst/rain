#include "cirrus/code/module.hpp"

#include <llvm/IR/LegacyPassManager.h>
#include <llvm/MC/TargetRegistry.h>
#include <llvm/Passes/PassBuilder.h>
#include <llvm/Passes/StandardInstrumentations.h>
#include <llvm/Support/MemoryBuffer.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/Target/TargetMachine.h>
#include <llvm/Transforms/InstCombine/InstCombine.h>
#include <llvm/Transforms/Scalar.h>
#include <llvm/Transforms/Scalar/GVN.h>
#include <llvm/Transforms/Scalar/Reassociate.h>
#include <llvm/Transforms/Scalar/SimplifyCFG.h>

#include "cirrus/ast/expr/all.hpp"
#include "cirrus/ast/type/all.hpp"
#include "cirrus/err/simple.hpp"

namespace cirrus::code {

namespace {

std::unique_ptr<llvm::TargetMachine> get_wasm_target_machine() {
    // std::cout << "Available targets:\n";
    // std::for_each(llvm::TargetRegistry::targets().begin(), llvm::TargetRegistry::targets().end(),
    //               [](const llvm::Target& target) { std::cout << target.getName() << "\n"; });
    // outputs:
    // wasm64
    // wasm32

    std::string error;

    std::string         target_triple = llvm::Triple::normalize("wasm32-unknown-unknown");
    const llvm::Target* target        = llvm::TargetRegistry::lookupTarget(target_triple, error);
    if (target == nullptr) {
        std::cerr << "failed to lookup target: " << error << "\n";
        std::abort();
    }

    std::string cpu;
    std::string features =
        "+simd128,+sign-ext,+bulk-memory,+mutable-globals,+nontrapping-fptoint,+multivalue";
    return std::unique_ptr<llvm::LLVMTargetMachine>(static_cast<llvm::LLVMTargetMachine*>(
        target->createTargetMachine(target_triple, cpu, features, llvm::TargetOptions(),
                                    std::nullopt, std::nullopt, llvm::CodeGenOptLevel::Default)));

    // std::string target_triple = llvm::Triple::normalize("wasm32-unknown-unknown");
    // WebAssemblyTargetMachine(const Target& T, const Triple& TT, StringRef CPU, StringRef FS,
    //                          const TargetOptions& Options, std::optional<Reloc::Model> RM,
    //                          std::optional<CodeModel::Model> CM, CodeGenOptLevel OL, bool JIT);
}

}  // namespace

Module::Module(std::shared_ptr<llvm::LLVMContext> ctx, const std::string_view name)
    : _ctx(std::move(ctx)), _mod(std::make_unique<llvm::Module>(name, *_ctx)) {
    _target_machine = get_wasm_target_machine();

    _mod->setDataLayout(_target_machine->createDataLayout());
    _mod->setTargetTriple(_target_machine->getTargetTriple().str());
}

void Module::optimize() {
    // Create new pass and analysis managers.
    auto lam  = llvm::LoopAnalysisManager();
    auto fam  = llvm::FunctionAnalysisManager();
    auto cgam = llvm::CGSCCAnalysisManager();
    auto mam  = llvm::ModuleAnalysisManager();
    auto pic  = llvm::PassInstrumentationCallbacks();
    auto si   = llvm::StandardInstrumentations(*_ctx, /*DebugLogging*/ false);
    si.registerCallbacks(pic, &mam);

    auto fpm = llvm::FunctionPassManager();
    // Do simple "peephole" and bit-twiddling optimizations
    fpm.addPass(llvm::InstCombinePass());
    // Reassociate expressions
    fpm.addPass(llvm::ReassociatePass());
    // Eliminate common sub-expressions
    fpm.addPass(llvm::GVNPass());
    // Simplify the control flow graph (deleting unreachable blocks, etc)
    fpm.addPass(llvm::SimplifyCFGPass());

    // Run a few optimization passes across the entire module
    llvm::PassBuilder pb;
    pb.registerModuleAnalyses(mam);
    pb.registerCGSCCAnalyses(cgam);
    pb.registerFunctionAnalyses(fam);
    pb.registerLoopAnalyses(lam);
    pb.crossRegisterProxies(lam, fam, cgam, mam);

    llvm::ModulePassManager mpm = pb.buildPerModuleDefaultPipeline(llvm::OptimizationLevel::Os,
                                                                   /*LTOPreLink*/ false);
    mpm.addPass(llvm::createModuleToFunctionPassAdaptor(std::move(fpm)));
    mpm.run(*_mod, mam);
}

util::Result<std::string> Module::emit_ir() const {
    std::string              str;
    llvm::raw_string_ostream os(str);
    _mod->print(os, nullptr);
    return OK(std::string, os.str());
}

util::Result<std::unique_ptr<llvm::MemoryBuffer>> Module::emit_obj() const {
    llvm::SmallString<0>      code;
    llvm::raw_svector_ostream ostream(code);
    llvm::legacy::PassManager pass_manager;
    if (_target_machine->addPassesToEmitFile(pass_manager, ostream, nullptr,
                                             llvm::CodeGenFileType::ObjectFile)) {
        std::abort();
    }
    pass_manager.run(*_mod);

    // Based on the documentation for llvm::raw_svector_ostream, the underlying SmallString is
    // always up to date, so there is no need to call flush().
    // ostream.flush();
    return OK(std::unique_ptr<llvm::MemoryBuffer>,
              llvm::MemoryBuffer::getMemBufferCopy(code.str()));
}

}  // namespace cirrus::code
