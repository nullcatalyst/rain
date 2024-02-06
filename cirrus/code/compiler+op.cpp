#include "cirrus/code/compiler.hpp"
// ^ Keep at top

#include <llvm/ExecutionEngine/GenericValue.h>

#include "cirrus/ast/expr/all.hpp"
#include "cirrus/ast/type/all.hpp"
#include "cirrus/code/compiler.hpp"
#include "cirrus/err/simple.hpp"
#include "cirrus/util/before_return.hpp"

namespace cirrus::code {

util::Result<llvm::Value*> Compiler::build(Context&                             ctx,
                                           const ast::BinaryOperatorExpression& binop_expression) {
    auto llvm_lhs_result = build(ctx, binop_expression.lhs());
    FORWARD_ERROR_WITH_TYPE(llvm::Value*, llvm_lhs_result);
    auto llvm_rhs_result = build(ctx, binop_expression.rhs());
    FORWARD_ERROR_WITH_TYPE(llvm::Value*, llvm_rhs_result);

    switch (binop_expression.op()) {
        case ast::BinaryOperator::Add:
            return OK(llvm::Value*,
                      _llvm_ir.CreateAdd(llvm_lhs_result.unwrap(), llvm_rhs_result.unwrap()));
        case ast::BinaryOperator::Subtract:
            return OK(llvm::Value*,
                      _llvm_ir.CreateSub(llvm_lhs_result.unwrap(), llvm_rhs_result.unwrap()));
        case ast::BinaryOperator::Multiply:
            return OK(llvm::Value*,
                      _llvm_ir.CreateMul(llvm_lhs_result.unwrap(), llvm_rhs_result.unwrap()));
        case ast::BinaryOperator::Divide:
            return OK(llvm::Value*,
                      _llvm_ir.CreateSDiv(llvm_lhs_result.unwrap(), llvm_rhs_result.unwrap()));
        case ast::BinaryOperator::Modulo:
            return OK(llvm::Value*,
                      _llvm_ir.CreateSRem(llvm_lhs_result.unwrap(), llvm_rhs_result.unwrap()));
        case ast::BinaryOperator::Equal:
            return OK(llvm::Value*,
                      _llvm_ir.CreateICmpEQ(llvm_lhs_result.unwrap(), llvm_rhs_result.unwrap()));
        case ast::BinaryOperator::NotEqual:
            return OK(llvm::Value*,
                      _llvm_ir.CreateICmpNE(llvm_lhs_result.unwrap(), llvm_rhs_result.unwrap()));
        case ast::BinaryOperator::Less:
            return OK(llvm::Value*,
                      _llvm_ir.CreateICmpSLT(llvm_lhs_result.unwrap(), llvm_rhs_result.unwrap()));
        case ast::BinaryOperator::LessEqual:
            return OK(llvm::Value*,
                      _llvm_ir.CreateICmpSLE(llvm_lhs_result.unwrap(), llvm_rhs_result.unwrap()));
        case ast::BinaryOperator::Greater:
            return OK(llvm::Value*,
                      _llvm_ir.CreateICmpSGT(llvm_lhs_result.unwrap(), llvm_rhs_result.unwrap()));
        case ast::BinaryOperator::GreaterEqual:
            return OK(llvm::Value*,
                      _llvm_ir.CreateICmpSGE(llvm_lhs_result.unwrap(), llvm_rhs_result.unwrap()));
        default:
            return ERR_PTR(llvm::Value*, err::SimpleError, "not implemented");
    }
}

util::Result<llvm::Value*> Compiler::build(Context&                   ctx,
                                           const ast::ExecExpression& exec_expression) {
    if (!exec_expression.compile_time_able()) {
        // TODO: Emit warning
        return build(ctx, exec_expression.expression());
    }

    // TODO: Determine return type from expression.
    llvm::Type*         llvm_return_type = llvm::Type::getInt32Ty(*_llvm_ctx);
    llvm::FunctionType* llvm_function_type =
        llvm::FunctionType::get(llvm_return_type, llvm::ArrayRef<llvm::Type*>(), false);
    llvm::Function* llvm_function = llvm::Function::Create(
        llvm_function_type, llvm::Function::InternalLinkage, "#exec", ctx.llvm_mod);

    auto               llvm_prev_insert_block = _llvm_ir.GetInsertBlock();
    auto               llvm_prev_insert_point = _llvm_ir.GetInsertPoint();
    util::BeforeReturn reset_ir_position([&]() {
        if (llvm_prev_insert_block != nullptr) {
            _llvm_ir.SetInsertPoint(llvm_prev_insert_block, llvm_prev_insert_point);
        }
    });

    llvm::BasicBlock* llvm_entry_block =
        llvm::BasicBlock::Create(*_llvm_ctx, "entry", llvm_function);
    _llvm_ir.SetInsertPoint(llvm_entry_block);

    // TODO: This needs to be careful, it can only build constant expressions
    auto llvm_value = build(ctx, exec_expression.expression());
    FORWARD_ERROR_WITH_TYPE(llvm::Value*, llvm_value);
    _llvm_ir.CreateRet(llvm_value.unwrap());

    llvm::GenericValue llvm_return_value =
        ctx.llvm_engine.runFunction(llvm_function, llvm::ArrayRef<llvm::GenericValue>());

    llvm_function->eraseFromParent();

    return OK(llvm::Value*,
              llvm::ConstantInt::get(*_llvm_ctx,
                                     llvm::APInt(32, llvm_return_value.IntVal.getSExtValue())));
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

    return OK(llvm::Value*, _llvm_ir.CreateInBoundsGEP(llvm_lhs_result.unwrap(), gep_indices));
}
*/
}  // namespace cirrus::code
