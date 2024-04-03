#include "rain/lang/code/expr/all.hpp"

namespace rain::lang::code {

llvm::Value* compile_let(Context& ctx, ast::LetExpression& let) {
    llvm::Value* llvm_value = compile_any_expression(ctx, let.value());

    auto& llvm_ir     = ctx.llvm_builder();
    auto* llvm_type   = ctx.llvm_type(let.type());
    auto* llvm_alloca = llvm_ir.CreateAlloca(llvm_type, nullptr, let.name());
    ctx.set_llvm_value(let.variable(), llvm_alloca);
    llvm_ir.CreateStore(llvm_value, llvm_alloca);

    return llvm_value;
}

}  // namespace rain::lang::code
