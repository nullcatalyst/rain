#include <array>

#include "rain/lang/ast/type/type.hpp"  // OptionalType
#include "rain/lang/code/type/all.hpp"

namespace rain::lang::code {

llvm::Type* compile_optional_type(Context& ctx, ast::OptionalType& optional_type) {
    auto* llvm_wrapped_type = get_or_compile_type(ctx, optional_type.type());

    if (optional_type.type().kind() == serial::TypeKind::Interface) {
        // An interface value is a pair of values, <ptr to struct, ptr to vtbl>.
        // An optional interface value can assign the vtbl to null, rather than storing an extra
        // flag.
        return llvm_wrapped_type;
    }

    if (llvm_wrapped_type->isPointerTy()) {
        // An optional pointer can be represented as a null pointer, and doesn't need a separate
        // flag to indicate whether the optional has a value or not.
        return llvm_wrapped_type;
    }

    const std::array<llvm::Type*, 2> members{
        llvm_wrapped_type,
        llvm::IntegerType::getInt1Ty(ctx.llvm_context()),
    };
    return llvm::StructType::get(ctx.llvm_context(), members);
}

}  // namespace rain::lang::code
