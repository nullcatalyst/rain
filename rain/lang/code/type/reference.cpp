#include <array>

#include "rain/lang/ast/type/type.hpp"  // OptionalType
#include "rain/lang/code/type/all.hpp"

namespace rain::lang::code {

llvm::Type* compile_reference_type(Context& ctx, ast::ReferenceType& reference_type) {
    auto* llvm_wrapped_type = get_or_compile_type(ctx, reference_type.type());

    if (reference_type.type().kind() == serial::TypeKind::Interface) {
        // An interface value is a pair of values, <ptr to struct, ptr to vtbl>.
        // The only difference between an interface value and a reference to an interface value is
        // the mutability of the value.
        return llvm_wrapped_type;
    }

    return llvm_wrapped_type->getPointerTo();
}

}  // namespace rain::lang::code
