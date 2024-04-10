#include "rain/lang/ast/expr/binary_operator.hpp"

#include "rain/lang/ast/expr/identifier.hpp"
#include "rain/lang/code/expr/any.hpp"

namespace rain::lang::code {

llvm::Value* get_element_pointer(Context& ctx, ast::Expression& expression);

llvm::Value* compile_binary_operator(Context& ctx, ast::BinaryOperatorExpression& binary_operator) {
    if (binary_operator.op() == serial::BinaryOperatorKind::Assign) {
        llvm::Value* llvm_value_ptr = get_element_pointer(ctx, binary_operator.lhs());
        if (llvm_value_ptr == nullptr) {
            util::panic("failed to compile binary operator: unknown left hand side kind: ");
        }

        llvm::Value* llvm_value = compile_any_expression(ctx, binary_operator.rhs());
        ctx.llvm_builder().CreateStore(llvm_value, llvm_value_ptr);
        return llvm_value;
    }

    auto*                               method = binary_operator.method();
    llvm::SmallVector<llvm::Value*, 4U> llvm_values{
        compile_any_expression(ctx, binary_operator.lhs()),
        compile_any_expression(ctx, binary_operator.rhs()),
    };
    return method->build_call(ctx, llvm_values);
}

}  // namespace rain::lang::code
