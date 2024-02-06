#include <llvm/ExecutionEngine/GenericValue.h>
#include <llvm/MC/TargetRegistry.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/Target/TargetMachine.h>
#include <llvm/Transforms/Utils/Cloning.h>

#include "absl/strings/str_format.h"
#include "cirrus/ast/expr/all.hpp"
#include "cirrus/ast/type/all.hpp"
#include "cirrus/code/compiler.hpp"
#include "cirrus/err/simple.hpp"
#include "cirrus/util/before_return.hpp"

namespace cirrus::code {

util::Result<llvm::Type*> Compiler::find_or_build_type(Context& ctx, const ast::Type& type) {
    switch (type.kind()) {
        case ast::NodeKind::StructType: {
            const auto struct_type = ast::StructType::from(type);
            if (const auto name = struct_type.name_or_empty(); name.empty()) {
                auto llvm_struct_type = build(ctx, struct_type);
                FORWARD_ERROR_WITH_TYPE(llvm::Type*, llvm_struct_type);
                return OK(llvm::Type*, llvm_struct_type.unwrap());
            } else {
                if (llvm::Type* const llvm_type = ctx.scope.find_llvm_type(name);
                    llvm_type != nullptr) {
                    return OK(llvm::Type*, llvm_type);
                } else {
                    auto llvm_struct_type = build(ctx, struct_type);
                    FORWARD_ERROR_WITH_TYPE(llvm::Type*, llvm_struct_type);
                    ctx.scope.set_llvm_type(name, llvm_struct_type.unwrap());
                    return OK(llvm::Type*, llvm_struct_type.unwrap());
                }
            }
        }

        case ast::NodeKind::UnresolvedType: {
            const auto unresolved_type = ast::UnresolvedType::from(type);
            if (llvm::Type* const llvm_type = ctx.scope.find_llvm_type(unresolved_type.name());
                llvm_type == nullptr) {
                return ERR_PTR(llvm::Type*, err::SimpleError,
                               absl::StrFormat("unknown type: %s", unresolved_type.name()));
            } else {
                return OK(llvm::Type*, llvm_type);
            }
        }

        default:
            return ERR_PTR(llvm::Type*, err::SimpleError, "unknown type");
    }
}

util::Result<llvm::StructType*> Compiler::build(Context& ctx, const ast::StructType& struct_type) {
    auto llvm_struct_type = llvm::StructType::create(*_llvm_ctx, struct_type.name_or_empty());
    std::vector<llvm::Type*> llvm_field_types;
    for (const auto& field : struct_type.fields()) {
        auto llvm_argument_type_result = find_or_build_type(ctx, field.type);
        FORWARD_ERROR_WITH_TYPE(llvm::StructType*, llvm_argument_type_result);
        llvm_field_types.emplace_back(llvm_argument_type_result.unwrap());
    }
    llvm_struct_type->setBody(llvm_field_types);
    return OK(llvm::StructType*, llvm_struct_type);
}

}  // namespace cirrus::code
