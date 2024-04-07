#include "rain/lang/code/expr/all.hpp"

namespace rain::lang::code {

llvm::Value* compile_member(Context& ctx, ast::MemberExpression& member) {
    auto* llvm_owner = compile_any_expression(ctx, member.lhs());

    auto& struct_type  = static_cast<ast::StructType&>(*member.lhs().type());
    auto  member_index = struct_type.find_member(member.name()).value();

    auto& llvm_ir = ctx.llvm_builder();

    auto* llvm_owner_type = ctx.llvm_type(&struct_type);
    assert(llvm_owner_type != nullptr && "owner type is null");

    // The owner is a vector (think SIMD types like f32x4), so LLVM requires us to "extract the
    // element" instead of "extracting the value". So handle that here.
    if (llvm_owner->getType()->isVectorTy()) {
        return llvm_ir.CreateExtractElement(llvm_owner, member_index);
    }

    if (member.kind() == serial::ExpressionKind::Variable) {
        auto& identifier = static_cast<const ast::IdentifierExpression&>(member.lhs());

        auto* variable = identifier.variable();
        assert(variable != nullptr && "variable is null");

        // if (variable->mutable_()) {
        auto* llvm_member_type = ctx.llvm_type(struct_type.fields()[member_index].type);
        assert(llvm_member_type != nullptr && "member type is null");

        return llvm_ir.CreateLoad(
            llvm_member_type,
            llvm_ir.CreateStructGEP(llvm_owner_type, ctx.llvm_value(variable), member_index));
    }

    return llvm_ir.CreateExtractValue(llvm_owner, static_cast<unsigned int>(member_index));
}

}  // namespace rain::lang::code
