#include "rain/lang/code/compile/all.hpp"

namespace rain::lang::code {

llvm::Value* compile_unary_operator(Context& ctx, ast::UnaryOperatorExpression& unary_operator) {
    auto* method = unary_operator.method();

    llvm::SmallVector<llvm::Value*, 4U> llvm_values{
        compile_any_expression(ctx, unary_operator.rhs()),
    };

    return method->build_call(ctx.llvm_builder(), llvm_values);
}

}  // namespace rain::lang::code
