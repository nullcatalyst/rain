#include "rain/code/module.hpp"

#include "llvm/IR/LegacyPassManager.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/StandardInstrumentations.h"
#include "llvm/Support/MemoryBuffer.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/Transforms/InstCombine/InstCombine.h"
#include "llvm/Transforms/Scalar.h"
#include "llvm/Transforms/Scalar/GVN.h"
#include "llvm/Transforms/Scalar/Reassociate.h"
#include "llvm/Transforms/Scalar/SimplifyCFG.h"
#include "llvm/Transforms/Vectorize/LoadStoreVectorizer.h"
#include "llvm/Transforms/Vectorize/LoopVectorize.h"
#include "llvm/Transforms/Vectorize/SLPVectorizer.h"
#include "llvm/Transforms/Vectorize/VectorCombine.h"
#include "rain/ast/expr/all.hpp"
#include "rain/ast/type/all.hpp"
#include "rain/code/target.hpp"
#include "rain/err/simple.hpp"

namespace rain::code {

Module::Module(std::shared_ptr<llvm::LLVMContext> ctx, std::unique_ptr<llvm::Module> mod,
               Scope exported_scope)
    : _llvm_ctx(std::move(ctx)),
      _llvm_mod(std::move(mod)),
      _exported_scope(std::move(exported_scope)) {
    // _mod->setDataLayout(_target_machine->createDataLayout());
    // _mod->setTargetTriple(_target_machine->getTargetTriple().str());
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

    fpm.addPass(llvm::LoadStoreVectorizerPass());
    fpm.addPass(llvm::VectorCombinePass());
    fpm.addPass(llvm::SLPVectorizerPass());
    fpm.addPass(llvm::LoopVectorizePass());

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
    mpm.run(*_llvm_mod, mam);
}

util::Result<std::string> Module::emit_ir() const {
    std::string              str;
    llvm::raw_string_ostream os(str);
    _llvm_mod->print(os, nullptr);
    return os.str();
}

// util::Result<std::unique_ptr<llvm::MemoryBuffer>> Module::emit_obj() const {
//     llvm::SmallString<0>      code;
//     llvm::raw_svector_ostream ostream(code);
//     llvm::legacy::PassManager pass_manager;

//     auto target_machine = wasm_target_machine();
//     if (target_machine->addPassesToEmitFile(pass_manager, ostream, nullptr,
//                                             llvm::CodeGenFileType::CGFT_ObjectFile)) {
//         std::abort();
//     }
//     pass_manager.run(*_llvm_mod);

//     // Based on the documentation for llvm::raw_svector_ostream, the underlying SmallString is
//     // always up to date, so there is no need to call flush().
//     // ostream.flush();
//     return llvm::MemoryBuffer::getMemBufferCopy(code.str());
// }

}  // namespace rain::code
