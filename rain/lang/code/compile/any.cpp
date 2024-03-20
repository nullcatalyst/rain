#include "rain/lang/code/compile/all.hpp"

namespace rain::lang::code {

llvm::Value* compile_any_expression(Context& ctx, ast::Expression& expression) {
    switch (expression.kind()) {
        case serial::ExpressionKind::Integer:
            return compile_integer(ctx, static_cast<ast::IntegerExpression&>(expression));

        case serial::ExpressionKind::Variable:
            return compile_identifier(ctx, static_cast<ast::IdentifierExpression&>(expression));

        case serial::ExpressionKind::BinaryOperator:
            return compile_binary_operator(ctx,
                                           static_cast<ast::BinaryOperatorExpression&>(expression));

            // case serial::ExpressionKind::Member:
            //     return compile_member(ctx, static_cast<ast::MemberExpression&>(expression));

        case serial::ExpressionKind::Call:
            return compile_call(ctx, static_cast<ast::CallExpression&>(expression));

        case serial::ExpressionKind::Block:
            return compile_block(ctx, static_cast<ast::BlockExpression&>(expression));

        default:
            break;
    }

    return nullptr;
}

}  // namespace rain::lang::code
