#include "rain/lang/code/compile/all.hpp"

namespace rain::lang::code {

llvm::Function* compile_function(Context& ctx, ast::FunctionExpression& function) {
    llvm::FunctionType* llvm_type =
        reinterpret_cast<llvm::FunctionType*>(ctx.llvm_type(function.type()));
    assert(llvm_type != nullptr && "Function type not found");
    llvm::Function* llvm_function = llvm::Function::Create(
        llvm_type, llvm::Function::ExternalLinkage, function.name_or_empty(), ctx.llvm_module());

    if (function.is_named()) {
        llvm_function->addFnAttr(llvm::Attribute::get(
            llvm_function->getContext(), "wasm-export-name", function.name_or_empty()));
    }

    auto& ir         = ctx.llvm_builder();
    auto* prev_block = ir.GetInsertBlock();

    llvm::BasicBlock* llvm_block =
        llvm::BasicBlock::Create(ctx.llvm_context(), "entry", llvm_function);

    ir.SetInsertPoint(llvm_block);
    for (int i = 0; i < function.arguments().size(); ++i) {
        auto* argument = function.arguments()[i];
        ctx.set_llvm_value(argument, llvm_function->arg_begin() + i);
    }

    llvm::Value* llvm_return_value = compile_block(ctx, *function.block());
    if (function.return_type() == nullptr) {
        ctx.llvm_builder().CreateRetVoid();
    } else {
        ctx.llvm_builder().CreateRet(llvm_return_value);
    }

    ir.SetInsertPoint(prev_block);

    if (const auto* function_variable = function.variable(); function_variable != nullptr) {
        ctx.set_llvm_value(function_variable, llvm_function);
    }
    return llvm_function;
}

}  // namespace rain::lang::code
