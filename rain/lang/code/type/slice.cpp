#include "rain/lang/ast/type/type.hpp"  // ArrayType
#include "rain/lang/code/type/all.hpp"

namespace rain::lang::code {

llvm::Type* compile_slice_type(Context& ctx, ast::SliceType& slice_type) {
    auto* llvm_element_type = get_or_compile_type(ctx, slice_type.type());
    assert(llvm_element_type != nullptr && "failed to compile slice element type");
    auto* llvm_element_ptr_type = llvm_element_type->getPointerTo();
    assert(llvm_element_ptr_type != nullptr && "failed to compile slice element pointer type");

    const std::array<llvm::Type*, 2> slice_types{llvm_element_ptr_type, llvm_element_ptr_type};
    llvm::Type* llvm_type = llvm::StructType::get(ctx.llvm_context(), slice_types);
    ctx.set_llvm_type(&slice_type, llvm_type);
    return llvm_type;
}

}  // namespace rain::lang::code
