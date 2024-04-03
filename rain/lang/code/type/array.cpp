#include "rain/lang/ast/type/type.hpp"  // ArrayType
#include "rain/lang/code/type/all.hpp"

namespace rain::lang::code {

llvm::Type* compile_array_type(Context& ctx, ast::ArrayType& array_type) {
    llvm::Type* llvm_type =
        llvm::ArrayType::get(get_or_compile_type(ctx, array_type.type()), array_type.length());
    ctx.set_llvm_type(&array_type, llvm_type);
    return llvm_type;
}

}  // namespace rain::lang::code
