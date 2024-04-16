#include "rain/lang/ast/type/interface.hpp"

#include <array>

#include "absl/strings/str_cat.h"
#include "llvm/IR/Type.h"
#include "rain/lang/code/type/all.hpp"

namespace rain::lang::code {

llvm::Type* compile_interface_type(Context& ctx, ast::InterfaceType& interface_type) {
    // Pre-emptively create the interface vtbl type, so that it can be stored in the lookup table,
    // and used in the interface methods that take in a self parameter.
    auto* llvm_type = llvm::StructType::create(ctx.llvm_context(), interface_type.name());
    ctx.set_llvm_type(&interface_type, llvm_type);

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

    llvm_type->setBody(llvm_field_types, /*is_packed*/ false);
    return llvm_type;
}

}  // namespace rain::lang::code
