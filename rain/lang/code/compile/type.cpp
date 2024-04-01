#include "rain/lang/ast/type/struct.hpp"
#include "rain/lang/code/compile/all.hpp"

namespace rain::lang::code {

namespace {

llvm::Type* get_or_compile_type(Context& ctx, ast::Type& type) {
    llvm::Type* llvm_type = ctx.llvm_type(&type);
    if (llvm_type != nullptr) {
        return llvm_type;
    }

    llvm_type = compile_type(ctx, type);
    if (llvm_type != nullptr) {
        return llvm_type;
    }

    util::console_error("failed to compile type: ", type.name());
    std::abort();
}

llvm::Type* compile_function_type(Context& ctx, ast::FunctionType& function_type) {
    llvm::SmallVector<llvm::Type*, 4> llvm_argument_types;
    llvm_argument_types.reserve(function_type.argument_types().size());
    for (auto* argument_type : function_type.argument_types()) {
        llvm_argument_types.push_back(get_or_compile_type(ctx, *argument_type));
    }

    llvm::Type* llvm_return_type = function_type.return_type() != nullptr
                                       ? get_or_compile_type(ctx, *function_type.return_type())
                                       : llvm::Type::getVoidTy(ctx.llvm_context());

    llvm::Type* llvm_type = llvm::FunctionType::get(llvm_return_type, llvm_argument_types, false);
    ctx.set_llvm_type(&function_type, llvm_type);

    return llvm_type;
}

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

}  // namespace

llvm::Type* compile_type(Context& ctx, ast::Type& type) {
    switch (type.kind()) {
        case serial::TypeKind::Builtin:
            // These should already be specially handled.
            return ctx.llvm_type(&type);

        case serial::TypeKind::Function:
            return compile_function_type(ctx, static_cast<ast::FunctionType&>(type));

        case serial::TypeKind::Struct:
            return compile_struct_type(ctx, static_cast<ast::StructType&>(type));

        case serial::TypeKind::Meta:
            return nullptr;

        default:
            util::console_error("unimplemented: compile type ", type.kind());
            std::abort();
    }
}

}  // namespace rain::lang::code
