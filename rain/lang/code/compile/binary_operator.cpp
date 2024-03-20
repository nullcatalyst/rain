#include "rain/lang/code/compile/all.hpp"

namespace rain::lang::code {

llvm::Value* compile_binary_operator(Context& ctx, ast::BinaryOperatorExpression& binary_operator) {
    auto* method = binary_operator.method();

    llvm::SmallVector<llvm::Value*, 4U> llvm_values{
        compile_any_expression(ctx, binary_operator.lhs()),
        compile_any_expression(ctx, binary_operator.rhs()),
    };

    return method->build_call(ctx.llvm_builder(), llvm_values);
}

}  // namespace rain::lang::code
