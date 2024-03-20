#include "rain/lang/code/compile/all.hpp"

namespace rain::lang::code {

llvm::Value* compile_identifier(Context& ctx, ast::IdentifierExpression& identifier) {
    return ctx.llvm_value(identifier.variable());
}

}  // namespace rain::lang::code
