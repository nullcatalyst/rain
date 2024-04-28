#include "rain/lang/ast/expr/boolean.hpp"

#include "llvm/IR/Constants.h"
#include "rain/lang/code/context.hpp"

namespace rain::lang::code {

llvm::Value* compile_boolean(Context& ctx, ast::BooleanExpression& boolean) {
    return ctx.llvm_builder().getInt1(boolean.value());
}

}  // namespace rain::lang::code
