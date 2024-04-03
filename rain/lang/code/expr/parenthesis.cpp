#include "rain/lang/code/expr/all.hpp"

namespace rain::lang::code {

llvm::Value* compile_parenthesis(Context& ctx, ast::ParenthesisExpression& parenthesis) {
    return compile_any_expression(ctx, parenthesis.expression());
}

}  // namespace rain::lang::code
