#include "rain/lang/ast/expr/binary_operator.hpp"

#include "rain/lang/ast/expr/identifier.hpp"
#include "rain/lang/code/expr/any.hpp"

namespace rain::lang::code {

llvm::Value* get_element_pointer(Context& ctx, ast::Expression& expression);

llvm::Value* compile_call_method(Context& ctx, ast::Expression& callee,
                                 ast::FunctionVariable&           method,
                                 llvm::ArrayRef<ast::Expression*> arguments);

llvm::Value* compile_binary_operator(Context& ctx, ast::BinaryOperatorExpression& binary_operator) {
    auto& llvm_ir = ctx.llvm_builder();

    if (binary_operator.op() == serial::BinaryOperatorKind::Assign) {
        llvm::Value* llvm_value_ptr = get_element_pointer(ctx, binary_operator.lhs());
        if (llvm_value_ptr == nullptr) {
            util::panic("failed to compile binary operator: unknown left hand side kind: ");
        }

        llvm::Value* llvm_value = compile_any_expression(ctx, binary_operator.rhs());
        if (binary_operator.rhs().type()->kind() == serial::TypeKind::Array) {
            auto& llvm_data_layout  = ctx.llvm_engine().getDataLayout();
            auto* llvm_element_type = llvm_value->getType()->getArrayElementType();
            auto  llvm_alignment    = llvm_data_layout.getABITypeAlign(llvm_element_type);
            auto  llvm_sizeof =
                llvm_ir.getInt64(llvm_data_layout.getTypeAllocSize(llvm_element_type));

            llvm_ir.CreateMemCpy(llvm_value_ptr, llvm_alignment, llvm_value, llvm_alignment,
                                 llvm_sizeof);
        } else {
            llvm_ir.CreateStore(llvm_value, llvm_value_ptr);
        }
        return llvm_value;
    }

    return compile_call_method(ctx, binary_operator.lhs(), *binary_operator.method(),
                               {&binary_operator.rhs()});
}

}  // namespace rain::lang::code
