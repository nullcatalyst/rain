#include "rain/lang/ast/expr/cast.hpp"

#include "rain/lang/code/expr/any.hpp"

namespace rain::lang::code {

llvm::Value* compile_call(Context& ctx, ast::Expression& callee, ast::FunctionVariable& method,
                          llvm::ArrayRef<ast::Expression*> arguments);

llvm::Value* compile_cast(Context& ctx, ast::CastExpression& cast) {
    std::vector<llvm::Value*> llvm_arguments;
    llvm_arguments.reserve(1);
    llvm_arguments.emplace_back(compile_any_expression(ctx, cast.expression()));
    return cast.method()->build_call(ctx, llvm_arguments);
}

}  // namespace rain::lang::code
