#include "rain/lang/ast/type/type.hpp"

#include "rain/lang/ast/type/function.hpp"
#include "rain/lang/ast/type/interface.hpp"
#include "rain/lang/ast/type/struct.hpp"
#include "rain/lang/code/type/all.hpp"
#include "rain/util/console.hpp"

namespace rain::lang::code {

llvm::Type* get_or_compile_type(Context& ctx, ast::Type& type) {
    llvm::Type* llvm_type = ctx.llvm_type(&type);
    if (llvm_type != nullptr) {
        return llvm_type;
    }

    llvm_type = compile_type(ctx, type);
    if (llvm_type == nullptr && type.kind() != serial::TypeKind::Meta) {
        util::panic("failed to compile type: ", type.display_name());
    }
    return llvm_type;
}

llvm::Type* compile_type(Context& ctx, ast::Type& type) {
    switch (type.kind()) {
        case serial::TypeKind::Builtin:
            // These should already be specially handled.
            return ctx.llvm_type(&type);

        case serial::TypeKind::Function:
            return compile_function_type(ctx, static_cast<ast::FunctionType&>(type));

        case serial::TypeKind::Struct:
            return compile_struct_type(ctx, static_cast<ast::StructType&>(type));

        case serial::TypeKind::Interface:
            return compile_interface_type(ctx, static_cast<ast::InterfaceType&>(type));

        case serial::TypeKind::Array:
            return compile_array_type(ctx, static_cast<ast::ArrayType&>(type));

        case serial::TypeKind::Optional:
            return compile_optional_type(ctx, static_cast<ast::OptionalType&>(type));

        case serial::TypeKind::Reference:
            return compile_reference_type(ctx, static_cast<ast::ReferenceType&>(type));

        case serial::TypeKind::Slice:
            return compile_slice_type(ctx, static_cast<ast::SliceType&>(type));

        case serial::TypeKind::Meta:
            return nullptr;

        default:
            util::panic("unimplemented: compile type ", type.kind());
    }
}

}  // namespace rain::lang::code
