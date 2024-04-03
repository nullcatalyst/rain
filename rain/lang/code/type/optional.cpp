#include "rain/lang/ast/type/type.hpp"  // OptionalType
#include "rain/lang/code/type/all.hpp"

namespace rain::lang::code {

llvm::Type* compile_optional_type(Context& ctx, ast::OptionalType& optional_type) {
    util::panic("unimplemented: compile optional type");
}

}  // namespace rain::lang::code
