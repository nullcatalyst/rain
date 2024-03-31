#include "rain/lang/code/compile/all.hpp"

namespace rain::lang::code {

llvm::Value* compile_extern(Context& ctx, ast::ExternExpression& extern_) {
    // TODO: Make sure this is actually a function, in case we add other externable things.

    llvm::Function* llvm_function = compile_function_declaration(ctx, extern_.declaration());

    llvm_function->setLinkage(llvm::Function::ExternalLinkage);

    const auto& keys = extern_.keys();
    if (keys[0] == "js") {
        llvm_function->addFnAttr(
            llvm::Attribute::get(ctx.llvm_context(), "wasm-import-module", keys[1]));
        llvm_function->addFnAttr(
            llvm::Attribute::get(ctx.llvm_context(), "wasm-import-name", keys[2]));
    }

    return llvm_function;
}

}  // namespace rain::lang::code
