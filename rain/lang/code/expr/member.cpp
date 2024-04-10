#include "rain/lang/code/expr/all.hpp"

namespace rain::lang::code {

llvm::Value* compile_member(Context& ctx, ast::MemberExpression& member) {
    auto* llvm_owner = compile_any_expression(ctx, member.lhs());

    auto* owner_type = member.lhs().type();
    if (owner_type->kind() == serial::TypeKind::Reference) {
        owner_type = &static_cast<ast::ReferenceType&>(*owner_type).type();
    }
    assert(owner_type->kind() == serial::TypeKind::Struct);
    auto& struct_type  = static_cast<ast::StructType&>(*owner_type);
    auto  member_index = struct_type.find_member(member.name()).value();

    auto& llvm_ir = ctx.llvm_builder();

    auto* llvm_owner_type = ctx.llvm_type(&struct_type);
    assert(llvm_owner_type != nullptr && "owner type is null");

    // The owner is a vector (think SIMD types like f32x4), so LLVM requires us to "extract the
    // element" instead of "extracting the value". So handle that here.
    if (llvm_owner->getType()->isVectorTy()) {
        return llvm_ir.CreateExtractElement(llvm_owner, member_index);
    }

    llvm::Value* llvm_ptr = get_element_pointer(ctx, member.lhs());
    if (llvm_ptr == nullptr) {
        return llvm_ir.CreateExtractValue(llvm_owner, static_cast<unsigned int>(member_index));
    }

    auto* llvm_member_type = ctx.llvm_type(struct_type.fields()[member_index].type);
    assert(llvm_member_type != nullptr && "member type is null");
    return llvm_ir.CreateLoad(llvm_member_type, llvm_ptr);
}

}  // namespace rain::lang::code
