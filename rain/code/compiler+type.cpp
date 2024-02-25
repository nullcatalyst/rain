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
    switch (type->kind()) {
        case ast::TypeKind::StructType: {
            const ast::StructType& struct_type = *std::static_pointer_cast<ast::StructType>(type);
            if (const auto name = struct_type.name_or_empty(); name.empty()) {
                auto llvm_struct_type = build(ctx, struct_type);
                FORWARD_ERROR(llvm_struct_type);
                return std::move(llvm_struct_type).value();
            } else {
                if (llvm::Type* llvm_type = ctx.scope.find_llvm_type(name); llvm_type != nullptr) {
                    return llvm_type;
                } else {
                    auto llvm_struct_type = build(ctx, struct_type);
                    FORWARD_ERROR(llvm_struct_type);
                    llvm_type = std::move(llvm_struct_type).value();
                    ctx.scope.set_llvm_type(name, llvm_type);
                    return llvm_type;
                }
            }
        }

        case ast::TypeKind::UnresolvedType: {
            const ast::UnresolvedType& unresolved_type =
                *std::static_pointer_cast<ast::UnresolvedType>(type);
            if (llvm::Type* const llvm_type = ctx.scope.find_llvm_type(unresolved_type.name());
                llvm_type == nullptr) {
                return ERR_PTR(err::SimpleError,
                               absl::StrFormat("unknown type: %s", unresolved_type.name()));
            } else {
                return llvm_type;
            }
        }

        default:
            return ERR_PTR(err::SimpleError, "unknown type");
    }
}

util::Result<llvm::StructType*> Compiler::build(Context& ctx, const ast::StructType& struct_type) {
    auto llvm_struct_type = llvm::StructType::create(*_llvm_ctx, struct_type.name_or_empty());
    std::vector<llvm::Type*> llvm_field_types;
    for (const auto& field : struct_type.fields()) {
        auto llvm_argument_type = find_or_build_type(ctx, field.type);
        FORWARD_ERROR(llvm_argument_type);
        llvm_field_types.emplace_back(std::move(llvm_argument_type).value());
    }
    llvm_struct_type->setBody(llvm_field_types);
    return llvm_struct_type;
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
            return ERR_PTR(err::SimpleError, "unknown type");
    }
}

}  // namespace rain::code