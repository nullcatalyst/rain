#include "rain/lang/ast/type/type.hpp"
#include "rain/lang/ast/var/unwrapped_optional.hpp"
#include "rain/lang/code/expr/all.hpp"
#include "rain/lang/code/type/all.hpp"

namespace rain::lang::code {

llvm::Value* compile_identifier(Context& ctx, ast::IdentifierExpression& identifier) {
    auto* variable = identifier.variable();
    assert(variable != nullptr && "variable is null");

    auto&        llvm_ir    = ctx.llvm_builder();
    llvm::Value* llvm_value = ctx.llvm_value(variable);

    if (variable->kind() == serial::VariableKind::UnwrappedOptional) {
        llvm_value =
            ctx.llvm_value(static_cast<ast::UnwrappedOptionalVariable*>(variable)->variable());
        llvm_value = llvm_ir.CreateExtractValue(llvm_value, 0);
    }

    if (variable->mutable_() && variable->type()->kind() != serial::TypeKind::Array) {
        return llvm_ir.CreateLoad(get_or_compile_type(ctx, *identifier.type()), llvm_value);
    }

    return llvm_value;
}

}  // namespace rain::lang::code
