#include <llvm/ExecutionEngine/GenericValue.h>
#include <llvm/MC/TargetRegistry.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/Target/TargetMachine.h>
#include <llvm/Transforms/Utils/Cloning.h>

#include "absl/strings/str_format.h"
#include "rain/ast/expr/all.hpp"
#include "rain/ast/type/all.hpp"
#include "rain/code/compiler.hpp"
#include "rain/err/simple.hpp"

namespace rain::code {

util::Result<llvm::Type*> Compiler::find_or_build_type(Context& ctx, const ast::TypePtr& type) {
    auto resolved_type = ctx.scope.resolve_type(type);
    if (resolved_type == nullptr) {
        return ERR_PTR(err::SimpleError, "unknown type (A)");
    }

    const auto llvm_type = ctx.scope.find_llvm_type(resolved_type);
    if (llvm_type != nullptr) {
        return llvm_type;
    }

    switch (type->kind()) {
        case ast::TypeKind::StructType: {
            const auto& struct_type      = *static_cast<const ast::StructType*>(type.get());
            auto        llvm_struct_type = build(ctx, struct_type);
            FORWARD_ERROR(llvm_struct_type);
            return llvm_type;
        }

        default:
            return ERR_PTR(err::SimpleError, "unknown type (B)");
    }
}

util::Result<llvm::StructType*> Compiler::build(Context& ctx, const ast::StructType& struct_type) {
    const auto name = struct_type.name_or_empty();

    llvm::StructType*        llvm_type = llvm::StructType::create(*_llvm_ctx, name);
    std::vector<llvm::Type*> llvm_field_types;
    for (const auto& field : struct_type.fields()) {
        const_cast<ast::StructTypeFieldData&>(field).type = ctx.scope.resolve_type(field.type);

        auto llvm_argument_type = find_or_build_type(ctx, field.type);
        FORWARD_ERROR(llvm_argument_type);
        llvm_field_types.emplace_back(std::move(llvm_argument_type).value());
    }
    llvm_type->setBody(llvm_field_types);

    ast::TypePtr type = const_cast<ast::StructType&>(struct_type).shared_from_this();
    if (!name.empty()) {
        ctx.scope.set_named_type(name, type);
    }

    ctx.scope.set_llvm_type(type, llvm_type);
    return llvm_type;
}

util::Result<llvm::Type*> Compiler::build(
    Context& ctx, const ast::TypeDeclarationExpression& type_declaration_expresion) {
    const ast::TypePtr& type = type_declaration_expresion.type();
    switch (type->kind()) {
        case ast::TypeKind::StructType: {
            const ast::StructType& struct_type = *static_cast<const ast::StructType*>(type.get());
            auto                   llvm_type   = build(ctx, struct_type);
            FORWARD_ERROR(llvm_type);
            return std::move(llvm_type).value();
        }

        default:
            return ERR_PTR(err::SimpleError, "unknown type (C)");
    }
}

}  // namespace rain::code
