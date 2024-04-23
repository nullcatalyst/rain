#include "rain/lang/code/expr/all.hpp"
#include "rain/lang/code/type/all.hpp"

namespace rain::lang::code {

llvm::Value* compile_let(Context& ctx, ast::LetExpression& let) {
    llvm::Value* llvm_value = compile_any_expression(ctx, let.value());

    auto& llvm_ir   = ctx.llvm_builder();
    auto* llvm_type = ctx.llvm_type(let.type());

    if (!let.global()) [[likely]] {
        auto* llvm_alloca = llvm_ir.CreateAlloca(llvm_type, nullptr, let.name());
        ctx.set_llvm_value(let.variable(), llvm_alloca);

        if (let.value().type()->kind() == serial::TypeKind::Array) {
            auto& llvm_data_layout  = ctx.llvm_engine().getDataLayout();
            auto* llvm_array_type   = get_or_compile_type(ctx, *let.value().type());
            auto* llvm_element_type = llvm_array_type->getArrayElementType();
            auto  llvm_alignment    = llvm_data_layout.getABITypeAlign(llvm_element_type);
            auto  llvm_sizeof =
                llvm_ir.getInt32(llvm_data_layout.getTypeAllocSize(llvm_element_type));

            llvm_ir.CreateMemCpy(llvm_alloca, llvm_alignment, llvm_value, llvm_alignment,
                                 llvm_sizeof);
        } else {
            llvm_ir.CreateStore(llvm_value, llvm_alloca);
        }
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
