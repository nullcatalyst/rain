#include "rain/lang/code/expr/all.hpp"

namespace rain::lang::code {

llvm::Value* compile_binary_operator(Context& ctx, ast::BinaryOperatorExpression& binary_operator) {
    if (binary_operator.op() == serial::BinaryOperatorKind::Assign) {
        switch (binary_operator.lhs().kind()) {
            case serial::ExpressionKind::Member: {
                util::panic("failed to compile binary operator: member assignment not implemented");
            }

            case serial::ExpressionKind::Variable: {
                auto& lhs = reinterpret_cast<ast::IdentifierExpression&>(binary_operator.lhs());
                llvm::Value* llvm_alloca = ctx.llvm_value(lhs.variable());
                llvm::Value* rhs         = compile_any_expression(ctx, binary_operator.rhs());
                ctx.llvm_builder().CreateStore(rhs, llvm_alloca);
                return rhs;
            }

            default:
                util::panic("failed to compile binary operator: unknown left hand side kind: ");
        }
    }

    auto*                               method = binary_operator.method();
    llvm::SmallVector<llvm::Value*, 4U> llvm_values{
        compile_any_expression(ctx, binary_operator.lhs()),
        compile_any_expression(ctx, binary_operator.rhs()),
    };
    return method->build_call(ctx.llvm_builder(), llvm_values);
}

}  // namespace rain::lang::code
