#include "rain/lang/code/expr/all.hpp"

namespace rain::lang::code {

llvm::Value* compile_let(Context& ctx, ast::LetExpression& let) {
    llvm::Value* llvm_value = compile_any_expression(ctx, let.value());

    auto& llvm_ir   = ctx.llvm_builder();
    auto* llvm_type = ctx.llvm_type(let.type());

    if (!let.global()) [[likely]] {
        auto* llvm_alloca = llvm_ir.CreateAlloca(llvm_type, nullptr, let.name());
        ctx.set_llvm_value(let.variable(), llvm_alloca);
        llvm_ir.CreateStore(llvm_value, llvm_alloca);
    } else {
        auto* llvm_global =
            new llvm::GlobalVariable(ctx.llvm_module(), llvm_type, !let.variable()->mutable_(),
                                     llvm::GlobalVariable::LinkageTypes::InternalLinkage,
                                     static_cast<llvm::Constant*>(llvm_value), let.name());
        ctx.set_llvm_value(let.variable(), llvm_global);
    }

    return llvm_value;
}

}  // namespace rain::lang::code
