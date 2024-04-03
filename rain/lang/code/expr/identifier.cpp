#include "rain/lang/code/expr/all.hpp"

namespace rain::lang::code {

llvm::Value* compile_identifier(Context& ctx, ast::IdentifierExpression& identifier) {
    llvm::Value* llvm_value = ctx.llvm_value(identifier.variable());

    if (identifier.variable() != nullptr && identifier.variable()->mutable_()) {
        return ctx.llvm_builder().CreateLoad(ctx.llvm_type(identifier.type()), llvm_value);
    }

    return llvm_value;
}

}  // namespace rain::lang::code
