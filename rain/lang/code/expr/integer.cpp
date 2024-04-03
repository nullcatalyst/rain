#include "rain/lang/ast/expr/integer.hpp"

#include "llvm/IR/Constants.h"
#include "rain/lang/code/context.hpp"

namespace rain::lang::code {

llvm::Value* compile_integer(Context& ctx, ast::IntegerExpression& integer) {
    return llvm::ConstantInt::get(ctx.llvm_type(integer.type()), integer.value());
}

}  // namespace rain::lang::code
