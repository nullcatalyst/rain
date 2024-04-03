#include "rain/lang/ast/expr/method.hpp"
#include "rain/lang/code/expr/all.hpp"

namespace rain::lang::code {

llvm::Function* compile_function(Context& ctx, ast::FunctionExpression& function) {
    llvm::Function* llvm_function = compile_function_declaration(ctx, function);

    auto& ir         = ctx.llvm_builder();
    auto* prev_block = ir.GetInsertBlock();

    llvm::BasicBlock* llvm_block =
        llvm::BasicBlock::Create(ctx.llvm_context(), "entry", llvm_function);

    ir.SetInsertPoint(llvm_block);
    for (int i = 0; i < function.arguments().size(); ++i) {
        auto& argument = function.arguments()[i];
        ctx.set_llvm_value(argument.get_nonnull(), llvm_function->arg_begin() + i);
    }

    llvm::Value* llvm_return_value = compile_block(ctx, *function.block());
    if (function.return_type() == nullptr) {
        ctx.llvm_builder().CreateRetVoid();
    } else {
        ctx.llvm_builder().CreateRet(llvm_return_value);
    }

    ir.SetInsertPoint(prev_block);

    return llvm_function;
}

}  // namespace rain::lang::code
