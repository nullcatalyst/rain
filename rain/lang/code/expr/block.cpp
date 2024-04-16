#include "rain/lang/ast/expr/block.hpp"

#include "rain/lang/code/expr/any.hpp"

namespace rain::lang::code {

llvm::Function* compile_function_declaration(Context&               ctx,
                                             ast::FunctionVariable& function_variable);

llvm::Value* compile_block(Context& ctx, ast::BlockExpression& block) {
    block.scope().for_each_function([&ctx](ast::FunctionVariable& function_variable) {
        compile_function_declaration(ctx, function_variable);
    });

    llvm::Value* return_value = nullptr;
    for (const auto& expression : block.expressions()) {
        return_value = compile_any_expression(ctx, *expression);
    }
    return return_value;
}

}  // namespace rain::lang::code
