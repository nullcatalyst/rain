#include "cirrus/code/module.hpp"

#include <llvm/Passes/PassBuilder.h>
#include <llvm/Passes/StandardInstrumentations.h>
#include <llvm/Transforms/InstCombine/InstCombine.h>
#include <llvm/Transforms/Scalar.h>
#include <llvm/Transforms/Scalar/GVN.h>
#include <llvm/Transforms/Scalar/Reassociate.h>
#include <llvm/Transforms/Scalar/SimplifyCFG.h>

#include "cirrus/ast/expr/all.hpp"
#include "cirrus/ast/type/all.hpp"
#include "cirrus/err/simple.hpp"

namespace cirrus::code {

Module::Module(std::shared_ptr<llvm::LLVMContext> ctx, const std::string_view name)
    : _ctx(std::move(ctx)), _mod(std::make_unique<llvm::Module>(name, *_ctx)) {
    // Set the target triple for the module
    _mod->setDataLayout("e-m:e-p:32:32-i64:64-n32:64-S128");
    _mod->setTargetTriple("wasm32-unknown-unknown");
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

std::string Module::llvm_ir() const {
    std::string              str;
    llvm::raw_string_ostream os(str);
    _mod->print(os, nullptr);
    return os.str();
}

}  // namespace cirrus::code
