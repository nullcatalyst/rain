#include "rain/code/compiler.hpp"
// ^ Keep at top

#include <llvm/ExecutionEngine/GenericValue.h>

#include "rain/ast/expr/all.hpp"
#include "rain/ast/type/all.hpp"
#include "rain/code/compiler.hpp"
#include "rain/err/simple.hpp"
#include "rain/util/defer.hpp"

namespace rain::code {

util::Result<llvm::Value*> Compiler::build(Context&                             ctx,
                                           const ast::BinaryOperatorExpression& binop_expression) {
    auto llvm_lhs = build(ctx, binop_expression.lhs());
    FORWARD_ERROR(llvm_lhs);
    auto llvm_rhs = build(ctx, binop_expression.rhs());
    FORWARD_ERROR(llvm_rhs);

    // TODO: This is a temporary fix. We need to handle this properly.
    binop_expression.set_type(binop_expression.lhs()->type());

    switch (binop_expression.op_kind()) {
        case ast::BinaryOperatorKind::Add:
            return _llvm_ir.CreateAdd(std::move(llvm_lhs).value(), std::move(llvm_rhs).value());
        case ast::BinaryOperatorKind::Subtract:
            return _llvm_ir.CreateSub(std::move(llvm_lhs).value(), std::move(llvm_rhs).value());
        case ast::BinaryOperatorKind::Multiply:
            return _llvm_ir.CreateMul(std::move(llvm_lhs).value(), std::move(llvm_rhs).value());
        case ast::BinaryOperatorKind::Divide:
            return _llvm_ir.CreateSDiv(std::move(llvm_lhs).value(), std::move(llvm_rhs).value());
        case ast::BinaryOperatorKind::Modulo:
            return _llvm_ir.CreateSRem(std::move(llvm_lhs).value(), std::move(llvm_rhs).value());
        case ast::BinaryOperatorKind::Equal:
            return _llvm_ir.CreateICmpEQ(std::move(llvm_lhs).value(), std::move(llvm_rhs).value());
        case ast::BinaryOperatorKind::NotEqual:
            return _llvm_ir.CreateICmpNE(std::move(llvm_lhs).value(), std::move(llvm_rhs).value());
        case ast::BinaryOperatorKind::Less:
            return _llvm_ir.CreateICmpSLT(std::move(llvm_lhs).value(), std::move(llvm_rhs).value());
        case ast::BinaryOperatorKind::LessEqual:
            return _llvm_ir.CreateICmpSLE(std::move(llvm_lhs).value(), std::move(llvm_rhs).value());
        case ast::BinaryOperatorKind::Greater:
            return _llvm_ir.CreateICmpSGT(std::move(llvm_lhs).value(), std::move(llvm_rhs).value());
        case ast::BinaryOperatorKind::GreaterEqual:
            return _llvm_ir.CreateICmpSGE(std::move(llvm_lhs).value(), std::move(llvm_rhs).value());
        default:
            return ERR_PTR(err::SimpleError,
                           "cannot compile binary operation: unknown binary operator");
    }
}

util::Result<llvm::Value*> Compiler::build(Context&                   ctx,
                                           const ast::ExecExpression& exec_expression) {
    if (!exec_expression.compile_time_capable()) {
        // TODO: Emit warning
        return build(ctx, exec_expression.expression());
    }

    // TODO: Determine return type from expression.
    llvm::Type*         llvm_return_type = llvm::Type::getInt32Ty(*_llvm_ctx);
    llvm::FunctionType* llvm_function_type =
        llvm::FunctionType::get(llvm_return_type, llvm::ArrayRef<llvm::Type*>(), false);
    llvm::Function* llvm_function = llvm::Function::Create(
        llvm_function_type, llvm::Function::InternalLinkage, "#exec", ctx.llvm_mod);

    auto        llvm_prev_insert_block = _llvm_ir.GetInsertBlock();
    auto        llvm_prev_insert_point = _llvm_ir.GetInsertPoint();
    util::Defer reset_ir_position([&]() {
        if (llvm_prev_insert_block != nullptr) {
            _llvm_ir.SetInsertPoint(llvm_prev_insert_block, llvm_prev_insert_point);
        }
    });

    llvm::BasicBlock* llvm_entry_block =
        llvm::BasicBlock::Create(*_llvm_ctx, "entry", llvm_function);
    _llvm_ir.SetInsertPoint(llvm_entry_block);

    // TODO: This needs to be careful, it can only build constant expressions
    auto llvm_value = build(ctx, exec_expression.expression());
    FORWARD_ERROR(llvm_value);
    _llvm_ir.CreateRet(std::move(llvm_value).value());

    llvm::GenericValue llvm_return_value =
        ctx.llvm_engine.runFunction(llvm_function, llvm::ArrayRef<llvm::GenericValue>());

    llvm_function->eraseFromParent();

    return llvm::ConstantInt::get(*_llvm_ctx,
                                  llvm::APInt(32, llvm_return_value.IntVal.getSExtValue()));
}
/*
// TODO: Finish implementing this once we have a way to declare variables.
util::Result<llvm::Value*> Compiler::build(Context&                     ctx,
                                           const ast::MemberExpression& member_expression) {
    // TODO: Determine return type from expression.



    auto llvm_lhs_result = build(ctx, member_expression.expr());
    std::array<llvm::Value*, 2> gep_indices{
        llvm::ConstantInt::get(*_llvm_ctx, llvm::APInt(32, 0)),
        llvm::ConstantInt::get(*_llvm_ctx, llvm::APInt(32, 0)),
    };

    return OK(llvm::Value*, _llvm_ir.CreateInBoundsGEP(llvm_lhs_result.value(), gep_indices));
}
*/
}  // namespace rain::code
