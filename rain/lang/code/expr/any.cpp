#include "rain/lang/code/expr/all.hpp"
#include "rain/util/console.hpp"

namespace rain::lang::code {

llvm::Value* compile_any_expression(Context& ctx, ast::Expression& expression) {
    switch (expression.kind()) {
        case serial::ExpressionKind::Boolean:
            return compile_boolean(ctx, static_cast<ast::BooleanExpression&>(expression));

        case serial::ExpressionKind::Integer:
            return compile_integer(ctx, static_cast<ast::IntegerExpression&>(expression));

        case serial::ExpressionKind::Float:
            return compile_float(ctx, static_cast<ast::FloatExpression&>(expression));

        case serial::ExpressionKind::Variable:
            return compile_identifier(ctx, static_cast<ast::IdentifierExpression&>(expression));

        case serial::ExpressionKind::Null:
            return nullptr;

        case serial::ExpressionKind::StructLiteral:
            return compile_struct_literal(ctx,
                                          static_cast<ast::StructLiteralExpression&>(expression));

        case serial::ExpressionKind::ArrayLiteral:
            return compile_array_literal(ctx,
                                         static_cast<ast::ArrayLiteralExpression&>(expression));

        case serial::ExpressionKind::SliceLiteral:
            return compile_slice_literal(ctx,
                                         static_cast<ast::SliceLiteralExpression&>(expression));

        case serial::ExpressionKind::Let:
            return compile_let(ctx, static_cast<ast::LetExpression&>(expression));

        case serial::ExpressionKind::Function:
            return compile_function(ctx, static_cast<ast::FunctionExpression&>(expression));

        case serial::ExpressionKind::BinaryOperator:
            return compile_binary_operator(ctx,
                                           static_cast<ast::BinaryOperatorExpression&>(expression));

        case serial::ExpressionKind::UnaryOperator:
            return compile_unary_operator(ctx,
                                          static_cast<ast::UnaryOperatorExpression&>(expression));

        case serial::ExpressionKind::Member:
            return compile_member(ctx, static_cast<ast::MemberExpression&>(expression));

        case serial::ExpressionKind::Call:
            return compile_call(ctx, static_cast<ast::CallExpression&>(expression));

        case serial::ExpressionKind::Cast:
            return compile_cast(ctx, static_cast<ast::CastExpression&>(expression));

        case serial::ExpressionKind::CompileTime:
            return compile_compile_time(ctx, static_cast<ast::CompileTimeExpression&>(expression));

        case serial::ExpressionKind::Parenthesis:
            return compile_parenthesis(ctx, static_cast<ast::ParenthesisExpression&>(expression));

        case serial::ExpressionKind::Block:
            return compile_block(ctx, static_cast<ast::BlockExpression&>(expression));

        case serial::ExpressionKind::If:
            return compile_if(ctx, static_cast<ast::IfExpression&>(expression));

        case serial::ExpressionKind::While:
            return compile_while(ctx, static_cast<ast::WhileExpression&>(expression));

        case serial::ExpressionKind::Export:
            return compile_export(ctx, static_cast<ast::ExportExpression&>(expression));

        case serial::ExpressionKind::Extern:
            return compile_extern(ctx, static_cast<ast::ExternExpression&>(expression));

        case serial::ExpressionKind::Type:
            return nullptr;

        default:
            util::panic("failed to compile expression: unknown expression kind: ",
                        static_cast<int>(expression.kind()));
    }

    return nullptr;
}

}  // namespace rain::lang::code
