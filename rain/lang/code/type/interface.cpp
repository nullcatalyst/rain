#include "rain/lang/ast/type/interface.hpp"

#include <array>

#include "absl/strings/str_cat.h"
#include "rain/lang/code/type/all.hpp"

namespace rain::lang::code {

llvm::Type* compile_interface_type(Context& ctx, ast::InterfaceType& interface_type) {
    llvm::SmallVector<llvm::Type*, 4> llvm_method_types;
    llvm_method_types.reserve(interface_type.methods().size());
    for (const auto& method : interface_type.methods()) {
        llvm_method_types.push_back(get_or_compile_type(ctx, *method->type()));
    }

    llvm::Type* llvm_vtbl_type = llvm::StructType::create(
        ctx.llvm_context(), llvm_method_types, absl::StrCat(interface_type.display_name(), ".vtbl"),
        /*packed*/ false);

    const std::array<llvm::Type*, 2> llvm_field_types{
        llvm::PointerType::get(llvm_vtbl_type, /*address space*/ 0),
        llvm_vtbl_type->getPointerTo(),
    };
    auto* llvm_type = llvm::StructType::get(ctx.llvm_context(), llvm_field_types, /*packed*/ false);
    ctx.set_llvm_type(&interface_type, llvm_type);

    return llvm_type;
}

}  // namespace rain::lang::code
