#include "rain/lang/code/expr/all.hpp"

namespace rain::lang::code {

llvm::Value* compile_identifier(Context& ctx, ast::IdentifierExpression& identifier) {
    auto* variable = identifier.variable();
    assert(variable != nullptr && "variable is null");

    llvm::Value* llvm_value = ctx.llvm_value(variable);

    if (variable->mutable_() && variable->type()->kind() != serial::TypeKind::Array) {
        return ctx.llvm_builder().CreateLoad(ctx.llvm_type(identifier.type()), llvm_value);
    }

    return llvm_value;
}

}  // namespace rain::lang::code
