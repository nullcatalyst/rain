#include "rain/lang/code/expr/all.hpp"
#include "rain/lang/code/type/all.hpp"
#include "rain/lang/code/util/optional.hpp"

namespace rain::lang::code {

llvm::Value* compile_if(Context& ctx, ast::IfExpression& if_) {
    llvm::Value* llvm_condition = compile_any_expression(ctx, if_.condition());

    auto& llvm_ctx  = ctx.llvm_context();
    auto& llvm_ir   = ctx.llvm_builder();
    auto* llvm_type = get_or_compile_type(ctx, *if_.type());

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
    if (if_.type() != if_.then().type()) {
        llvm_then_result = util::create_optional_literal(ctx, llvm_type, llvm_then_result);
    }
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

    // We can't assume that the saved else block is the one that actually got us to the merge block.
    // For example, an if expression within the original else block results in a new else block.
    auto* llvm_found_else_block = llvm_ir.GetInsertBlock();
    llvm_ir.SetInsertPoint(llvm_merge_block);

    if (if_.has_else() && !ctx.returned()) {
        // TODO: This is a temporary hack to avoid properly having to support the case where the if
        // expression shuold return an optional value.
        if (llvm_then_result == nullptr || llvm_else_result == nullptr) {
            return nullptr;
        }

        llvm::Type* const llvm_type   = llvm_then_result->getType();
        llvm::PHINode*    llvm_result = llvm_ir.CreatePHI(llvm_type, 2);
        llvm_result->addIncoming(llvm_then_result, llvm_then_block);
        llvm_result->addIncoming(llvm_else_result, llvm_found_else_block);
        return llvm_result;
    }

    if (!if_.has_else() && !ctx.returned()) {
        llvm_else_result = llvm::Constant::getNullValue(llvm_type);

        llvm::PHINode* llvm_result = llvm_ir.CreatePHI(llvm_type, 2);
        llvm_result->addIncoming(llvm_then_result, llvm_then_block);
        llvm_result->addIncoming(llvm_else_result, llvm_found_else_block);
        return llvm_result;
    }

    return nullptr;
}

}  // namespace rain::lang::code
