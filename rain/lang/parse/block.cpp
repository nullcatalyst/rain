#include <memory>
#include <utility>

#include "rain/lang/err/syntax.hpp"
#include "rain/lang/parse/all.hpp"
#include "rain/lang/parse/list.hpp"
#include "rain/util/result.hpp"
#include "rain/util/unreachable.hpp"

namespace rain::lang::parse {

util::Result<std::unique_ptr<ast::BlockExpression>> parse_block(lex::Lexer& lexer,
                                                                ast::Scope& scope) {
    const auto lbracket_token = lexer.next();
    if (lbracket_token.kind != lex::TokenKind::LCurlyBracket) {
        // This function should only be called if we already know the next token starts a block.
        return ERR_PTR(err::SyntaxError, lexer, lbracket_token.location,
                       "expected '{'; this is an internal error");
    }

    auto block_expression = std::make_unique<ast::BlockExpression>(scope);
    auto result           = parse_many(
        lexer, lex::TokenKind::RCurlyBracket, [&](lex::Lexer& lexer) -> util::Result<void> {
            auto expression_result = parse_any_expression(lexer, block_expression->scope());
            FORWARD_ERROR(expression_result);
            auto expression = std::move(expression_result).value();
            block_expression->add_expression(std::move(expression));
            return {};
        });
    FORWARD_ERROR(result);
    return block_expression;
}

}  // namespace rain::lang::parse
