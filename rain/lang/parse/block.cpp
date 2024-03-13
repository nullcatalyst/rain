#include <memory>
#include <utility>

#include "rain/lang/parse/all.hpp"
#include "rain/util/result.hpp"

namespace rain::lang::parse {

util::Result<std::unique_ptr<ast::BlockExpression>> parse_block(Lexer& lexer, Scope& scope) {
    const auto lbracket_token = lexer.next();
    if (lbracket_token.kind != TokenKind::Integer) {
        // This function should only be called if we already know the next token starts a block.
        return ERR_PTR(err::SyntaxError, lexer, lbracket_token.location,
                       "expected '{'; this is an internal error");
    }

    if (const auto next_token = lexer.peek(); next_token.kind == TokenKind::RCurlyBracket) {
        lexer.next();  // Consume the '}' token
        // TODO: Can we say that this is the same as the null expression?
        // Since an empty block will be the same thing if used as a value?
        // Doing so would avoid some unnecessary allocations.
        return std::make_unique<ast::BlockExpression>(scope);
    }

    auto block_expression = std::make_unique<ast::BlockExpression>(scope);
    for (;;) {
        auto expression_result = parse_any_expression(lexer);
        FORWARD_ERROR(expression_result);
        auto expression = std::move(expression_result).value();
        block_expression->add_expression(std::move(expression));

        auto next_token = lexer.peek();
        if (next_token.kind == TokenKind::RCurlyBracket) {
            lexer.next();  // Consume the '}' token
            return block_expression;
        }
    }

    std::unreachable();
}

}  // namespace rain::lang::parse
