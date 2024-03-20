#include "rain/lang/code/context.hpp"

#include "rain/lang/code/target/default.hpp"

// LLVM Optimization Passes
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/StandardInstrumentations.h"
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

namespace rain::lang::code {

Context::Context()
    : _llvm_ctx(std::make_unique<llvm::LLVMContext>()),
      _llvm_target_machine(target_machine()),
      _llvm_module(std::make_unique<llvm::Module>("rain", *_llvm_ctx)),
      _llvm_builder(*_llvm_ctx) {}

void Context::set_llvm_type(const ast::Type* type, llvm::Type* llvm_type) {
    _llvm_types.emplace(type, llvm_type);
}

llvm::Type* Context::llvm_type(const ast::Type* type) const {
    if (const auto it = _llvm_types.find(type); it != _llvm_types.end()) {
        return it->second;
    }
    return nullptr;
}

void Context::set_llvm_value(const ast::Variable* variable, llvm::Value* llvm_value) {
    _llvm_values.emplace(variable, llvm_value);
}

llvm::Value* Context::llvm_value(const ast::Variable* variable) const {
    if (const auto it = _llvm_values.find(variable); it != _llvm_values.end()) {
        return it->second;
    }
    return nullptr;
}

void Context::optimize() {
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
    mpm.run(*_llvm_module, mam);
}

std::string Context::emit_ir() const {
    std::string              str;
    llvm::raw_string_ostream os(str);
    _llvm_module->print(os, nullptr);
    return os.str();
}

}  // namespace rain::lang::code
