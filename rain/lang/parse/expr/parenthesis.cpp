#include "rain/lang/ast/expr/parenthesis.hpp"

#include <memory>

#include "rain/lang/ast/scope/scope.hpp"
#include "rain/lang/err/syntax.hpp"
#include "rain/lang/lex/lexer.hpp"
#include "rain/util/result.hpp"

namespace rain::lang::parse {

util::Result<std::unique_ptr<ast::Expression>> parse_any_expression(lex::Lexer& lexer,
                                                                    ast::Scope& scope);

util::Result<std::unique_ptr<ast::ParenthesisExpression>> parse_parenthesis(lex::Lexer& lexer,
                                                                            ast::Scope& scope) {
    const auto lparen_token = lexer.next();
    IF_DEBUG {
        if (lparen_token.kind != lex::TokenKind::LRoundBracket) {
            return ERR_PTR(err::SyntaxError, lparen_token.location,
                           "expected '('; this is an internal error");
        }
    }

    auto expression = parse_any_expression(lexer, scope);
    FORWARD_ERROR(expression);

    const auto rparen_token = lexer.next();
    if (rparen_token.kind != lex::TokenKind::RRoundBracket) {
        return ERR_PTR(err::SyntaxError, lparen_token.location,
                       "expected ')' to end expression in parenthesis");
    }

    return std::make_unique<ast::ParenthesisExpression>(
        std::move(expression).value(), lparen_token.location.merge(rparen_token.location));
}

}  // namespace rain::lang::parse
