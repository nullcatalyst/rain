#include "rain/lang/code/module.hpp"

#include <string>

#include "llvm/IR/LegacyPassManager.h"
#include "llvm/MC/TargetRegistry.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/StandardInstrumentations.h"
#include "llvm/Support/MemoryBuffer.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/Transforms/InstCombine/InstCombine.h"
#include "llvm/Transforms/Scalar.h"
#include "llvm/Transforms/Scalar/GVN.h"
#include "llvm/Transforms/Scalar/Reassociate.h"
#include "llvm/Transforms/Scalar/SimplifyCFG.h"
// #include "llvm/Transforms/Vectorize/LoadStoreVectorizer.h"
// #include "llvm/Transforms/Vectorize/LoopVectorize.h"
// #include "llvm/Transforms/Vectorize/SLPVectorizer.h"
// #include "llvm/Transforms/Vectorize/VectorCombine.h"

namespace rain::lang::code {

namespace {

std::unique_ptr<llvm::TargetMachine> clone_target_machine(
    const llvm::TargetMachine& llvm_target_machine) {
    llvm_target_machine.getTargetTriple();
    std::string error;

    std::string         target_triple = llvm_target_machine.getTargetTriple().str();
    const llvm::Target* llvm_target   = &llvm_target_machine.getTarget();

    std::string cpu;
    std::string features = llvm_target_machine.getTargetFeatureString().str();
    return std::unique_ptr<llvm::TargetMachine>(
        llvm_target->createTargetMachine(target_triple, cpu, features, llvm::TargetOptions(),
                                         std::nullopt, std::nullopt, llvm::CodeGenOpt::Default));
}

}  // namespace

Module::Module(Options& options)
    : _llvm_ctx(std::make_unique<llvm::LLVMContext>()),
      _llvm_target_machine(options.create_target_machine()) {
    assert(_llvm_target_machine != nullptr && "failed to create target machine");

    auto llvm_module = std::make_unique<llvm::Module>("rain", *_llvm_ctx);
    llvm_module->setDataLayout(_llvm_target_machine->createDataLayout());
    llvm_module->setTargetTriple(_llvm_target_machine->getTargetTriple().str());

    _llvm_module = llvm_module.get();
    _llvm_engine =
        options.create_engine(std::move(llvm_module), clone_target_machine(*_llvm_target_machine));
}

void Module::optimize() {
    // Create new pass and analysis managers.
    auto lam  = llvm::LoopAnalysisManager();
    auto fam  = llvm::FunctionAnalysisManager();
    auto cgam = llvm::CGSCCAnalysisManager();
    auto mam  = llvm::ModuleAnalysisManager();
    auto pic  = llvm::PassInstrumentationCallbacks();
    auto si   = llvm::StandardInstrumentations(*_llvm_ctx, /*DebugLogging*/ false);
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

    // fpm.addPass(llvm::LoadStoreVectorizerPass());
    // fpm.addPass(llvm::VectorCombinePass());
    // fpm.addPass(llvm::SLPVectorizerPass());
    // fpm.addPass(llvm::LoopVectorizePass());

    // Run a few optimization passes across the entire module
    llvm::PassBuilder pb;
    pb.registerModuleAnalyses(mam);
    pb.registerCGSCCAnalyses(cgam);
    pb.registerFunctionAnalyses(fam);
    pb.registerLoopAnalyses(lam);
    pb.crossRegisterProxies(lam, fam, cgam, mam);

    llvm::ModulePassManager mpm = pb.buildPerModuleDefaultPipeline(llvm::OptimizationLevel::Os,
                                                                   /*LTOPreLink*/ true);
    // llvm::ModulePassManager mpm = pb.buildModuleInlinerPipeline(
    //     llvm::OptimizationLevel::Os, llvm::ThinOrFullLTOPhase::ThinLTOPreLink);
    mpm.addPass(llvm::createModuleToFunctionPassAdaptor(std::move(fpm)));
    mpm.run(*_llvm_module, mam);
}

util::Result<std::string> Module::emit_ir() const {
    std::string              str;
    llvm::raw_string_ostream os(str);
    _llvm_module->print(os, nullptr);
    return os.str();
}

util::Result<std::unique_ptr<llvm::MemoryBuffer>> Module::emit_obj() const {
    llvm::SmallString<0>      code;
    llvm::raw_svector_ostream ostream(code);
    llvm::legacy::PassManager pass_manager;

    auto target_machine = _llvm_target_machine.get();
    if (target_machine->addPassesToEmitFile(pass_manager, ostream, nullptr,
                                            llvm::CodeGenFileType::CGFT_ObjectFile)) {
        std::abort();
    }
    pass_manager.run(*_llvm_module);

    // Based on the documentation for llvm::raw_svector_ostream, the underlying SmallString is
    // always up to date, so there is no need to call flush() before returning the string.
    return llvm::MemoryBuffer::getMemBufferCopy(code.str());
}

}  // namespace rain::lang::code
