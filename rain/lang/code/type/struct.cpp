#include "rain/lang/ast/type/struct.hpp"

#include "rain/lang/code/type/all.hpp"

namespace rain::lang::code {

llvm::Type* compile_struct_type(Context& ctx, ast::StructType& struct_type) {
    llvm::SmallVector<llvm::Type*, 4> llvm_field_types;
    llvm_field_types.reserve(struct_type.fields().size());
    for (const auto& field : struct_type.fields()) {
        llvm_field_types.push_back(get_or_compile_type(ctx, *field.type.get_nonnull()));
    }

    llvm::Type* llvm_type =
        struct_type.is_named()
            ? llvm::StructType::create(ctx.llvm_context(), llvm_field_types,
                                       struct_type.name_or_empty(), /*packed*/ false)
            : llvm::StructType::get(ctx.llvm_context(), llvm_field_types, /*packed*/ false);
    ctx.set_llvm_type(&struct_type, llvm_type);

    return llvm_type;
}

}  // namespace rain::lang::code
