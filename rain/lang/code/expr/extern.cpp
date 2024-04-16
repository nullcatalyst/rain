#include "rain/lang/code/expr/all.hpp"

namespace rain::lang::code {

llvm::Value* compile_extern(Context& ctx, ast::ExternExpression& extern_) {
    // TODO: Make sure this is actually a function, in case we add other externable things.

    llvm::Function* llvm_function =
        compile_function_declaration(ctx, *extern_.declaration().variable());
    llvm_function->setLinkage(llvm::Function::ExternalLinkage);

    ctx.options().compile_extern_compile_time_runnable(
        ctx, llvm_function, std::span{extern_.keys().begin(), extern_.keys().size()});

    return llvm_function;
}

}  // namespace rain::lang::code
