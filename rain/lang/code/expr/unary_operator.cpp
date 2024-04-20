#include "llvm/ADT/SmallVector.h"
#include "llvm/IR/Value.h"
#include "rain/lang/code/context.hpp"
#include "rain/lang/code/expr/all.hpp"

namespace rain::lang::code {

llvm::Value* compile_call_method(Context& ctx, ast::Expression& callee,
                                 ast::FunctionVariable&           method,
                                 llvm::ArrayRef<ast::Expression*> arguments);

llvm::Value* compile_unary_operator(Context& ctx, ast::UnaryOperatorExpression& unary_operator) {
    return compile_call_method(ctx, unary_operator.expression(), *unary_operator.method(), {});
}

}  // namespace rain::lang::code
