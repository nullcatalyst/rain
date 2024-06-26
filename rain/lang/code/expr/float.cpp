#include "rain/lang/ast/expr/float.hpp"

#include "llvm/IR/Constants.h"
#include "rain/lang/code/context.hpp"

namespace rain::lang::code {

llvm::Value* compile_float(Context& ctx, ast::FloatExpression& float_) {
    return llvm::ConstantFP::get(ctx.llvm_type(float_.type()), float_.value());
}

}  // namespace rain::lang::code
