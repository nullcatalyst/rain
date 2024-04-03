#pragma once

#include "llvm/IR/Value.h"
#include "rain/lang/ast/expr/expression.hpp"
#include "rain/lang/code/context.hpp"

namespace rain::lang::code {

llvm::Value* compile_any_expression(Context& ctx, ast::Expression& expression);

}  // namespace rain::lang::code
