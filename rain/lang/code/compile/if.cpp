#include "rain/lang/code/compile/all.hpp"

namespace rain::lang::code {

llvm::Value* compile_if(Context& ctx, ast::IfExpression& if_) {
    llvm::Value* llvm_condition = compile_any_expression(ctx, if_.condition());

    auto& llvm_ctx = ctx.llvm_context();
    auto& llvm_ir  = ctx.llvm_builder();

    llvm::Function* const   llvm_function = llvm_ir.GetInsertBlock()->getParent();
    llvm::BasicBlock* const llvm_then_block =
        llvm::BasicBlock::Create(llvm_ctx, "then", llvm_function);
    llvm::BasicBlock* const llvm_else_block =
        llvm::BasicBlock::Create(llvm_ctx, "else", llvm_function);
    llvm::BasicBlock* const llvm_merge_block =
        llvm::BasicBlock::Create(llvm_ctx, "endif", llvm_function);

    llvm_ir.CreateCondBr(llvm_condition, llvm_then_block, llvm_else_block);

    llvm_ir.SetInsertPoint(llvm_then_block);
    llvm::Value* llvm_then_result = compile_block(ctx, if_.then());
    if (!ctx.returned()) {
        llvm_ir.CreateBr(llvm_merge_block);
    }
    const bool then_returned = ctx.returned();
    ctx.set_returned(false);

    llvm_ir.SetInsertPoint(llvm_else_block);

    llvm::Value* llvm_else_result = nullptr;
    if (if_.has_else()) {
        llvm_else_result = compile_block(ctx, if_.else_());
    }
    if (!ctx.returned()) {
        llvm_ir.CreateBr(llvm_merge_block);
    }

    ctx.set_returned(then_returned && ctx.returned());

    llvm_ir.SetInsertPoint(llvm_merge_block);

    llvm::Type* const llvm_type   = llvm_then_result->getType();
    llvm::PHINode*    llvm_result = llvm_ir.CreatePHI(llvm_type, 2);
    llvm_result->addIncoming(llvm_then_result, llvm_then_block);
    llvm_result->addIncoming(llvm_else_result, llvm_else_block);
    return llvm_result;
}

}  // namespace rain::lang::code
