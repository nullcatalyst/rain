#include "rain/lang/code/compile/all.hpp"

namespace rain::lang::code {

llvm::Value* compile_any_expression(Context& ctx, ast::Expression& expression) {
    switch (expression.kind()) {
        case serial::ExpressionKind::Integer:
            return compile_integer(ctx, static_cast<ast::IntegerExpression&>(expression));

        case serial::ExpressionKind::Float:
            return compile_float(ctx, static_cast<ast::FloatExpression&>(expression));

        case serial::ExpressionKind::Variable:
            return compile_identifier(ctx, static_cast<ast::IdentifierExpression&>(expression));

        case serial::ExpressionKind::Let:
            return compile_let(ctx, static_cast<ast::LetExpression&>(expression));

        case serial::ExpressionKind::BinaryOperator:
            return compile_binary_operator(ctx,
                                           static_cast<ast::BinaryOperatorExpression&>(expression));

        case serial::ExpressionKind::UnaryOperator:
            return compile_unary_operator(ctx,
                                          static_cast<ast::UnaryOperatorExpression&>(expression));

            // case serial::ExpressionKind::Member:
            //     return compile_member(ctx, static_cast<ast::MemberExpression&>(expression));

        case serial::ExpressionKind::Call:
            return compile_call(ctx, static_cast<ast::CallExpression&>(expression));

        case serial::ExpressionKind::CompileTime:
            return compile_compile_time(ctx, static_cast<ast::CompileTimeExpression&>(expression));

        case serial::ExpressionKind::Parenthesis:
            return compile_parenthesis(ctx, static_cast<ast::ParenthesisExpression&>(expression));

        case serial::ExpressionKind::Block:
            return compile_block(ctx, static_cast<ast::BlockExpression&>(expression));

        case serial::ExpressionKind::If:
            return compile_if(ctx, static_cast<ast::IfExpression&>(expression));

        case serial::ExpressionKind::Export:
            return compile_export(ctx, static_cast<ast::ExportExpression&>(expression));

        default:
            util::console_error("failed to compile expression: unknown expression kind: ",
                                static_cast<int>(expression.kind()));
            std::abort();
    }

    return nullptr;
}

}  // namespace rain::lang::code
