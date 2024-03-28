#include "rain/lang/code/compile/all.hpp"

namespace rain::lang::code {

llvm::Value* compile_export(Context& ctx, ast::ExportExpression& export_) {
    // TODO: Make sure this is actually a function, in case we add other exportable things
    ast::FunctionExpression& function =
        reinterpret_cast<ast::FunctionExpression&>(export_.expression());

    llvm::Function* llvm_function = compile_function(ctx, function);

    llvm_function->setLinkage(llvm::Function::ExternalLinkage);
    llvm_function->addFnAttr(llvm::Attribute::get(llvm_function->getContext(), "wasm-export-name",
                                                  llvm_function->getName()));

    return llvm_function;
}

}  // namespace rain::lang::code
