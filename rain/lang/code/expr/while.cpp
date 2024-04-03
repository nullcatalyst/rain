#include "rain/lang/code/expr/all.hpp"

namespace rain::lang::code {

llvm::Value* compile_while(Context& ctx, ast::WhileExpression& while_) {
    llvm::Value* llvm_condition = compile_any_expression(ctx, while_.condition());

    auto& llvm_ctx = ctx.llvm_context();
    auto& llvm_ir  = ctx.llvm_builder();

    llvm::Function* const   llvm_function = llvm_ir.GetInsertBlock()->getParent();
    llvm::BasicBlock* const llvm_loop_block =
        llvm::BasicBlock::Create(llvm_ctx, "loop", llvm_function);
    llvm::BasicBlock* const llvm_else_block =
        llvm::BasicBlock::Create(llvm_ctx, "else", llvm_function);
    llvm::BasicBlock* const llvm_merge_block =
        llvm::BasicBlock::Create(llvm_ctx, "endwhile", llvm_function);

    llvm_ir.CreateCondBr(llvm_condition, llvm_loop_block, llvm_else_block);

    llvm_ir.SetInsertPoint(llvm_loop_block);
    llvm::Value* llvm_loop_result = compile_block(ctx, while_.loop());
    if (!ctx.returned()) {
        llvm::Value* llvm_loop_again_condition = compile_any_expression(ctx, while_.condition());
        llvm_ir.CreateCondBr(llvm_loop_again_condition, llvm_loop_block, llvm_merge_block);
    }
    const bool loop_returned = ctx.returned();
    ctx.set_returned(false);

    llvm_ir.SetInsertPoint(llvm_else_block);

    llvm::Value* llvm_else_result = nullptr;
    if (while_.has_else()) {
        llvm_else_result = compile_block(ctx, while_.else_());
    }
    if (!ctx.returned()) {
        llvm_ir.CreateBr(llvm_merge_block);
    }

    ctx.set_returned(loop_returned && ctx.returned());

    llvm_ir.SetInsertPoint(llvm_merge_block);

    if (while_.has_else() && !ctx.returned()) {
        llvm::Type* const llvm_type   = llvm_else_result->getType();
        llvm::PHINode*    llvm_result = llvm_ir.CreatePHI(llvm_type, 2);
        llvm_result->addIncoming(llvm_loop_result, llvm_loop_block);
        llvm_result->addIncoming(llvm_else_result, llvm_else_block);
        return llvm_result;
    } else {
        return nullptr;
    }
}

}  // namespace rain::lang::code
