#include "rain/lang/code/compile/all.hpp"

namespace rain::lang::code {

llvm::Value* compile_extern(Context& ctx, ast::ExternExpression& extern_) {
    // TODO: Make sure this is actually a function, in case we add other externable things.

    llvm::Function* llvm_function = compile_function_declaration(ctx, extern_.declaration());

    llvm_function->setLinkage(llvm::Function::ExternalLinkage);
    llvm_function->addFnAttr(llvm::Attribute::get(llvm_function->getContext(), "wasm-export-name",
                                                  llvm_function->getName()));

    return llvm_function;
}

}  // namespace rain::lang::code
