#include "rain/lang/ast/expr/if.hpp"

#include <memory>

#include "rain/lang/ast/scope/scope.hpp"
#include "rain/lang/err/syntax.hpp"
#include "rain/lang/lex/lexer.hpp"
#include "rain/lang/parse/all.hpp"
#include "rain/util/result.hpp"

namespace rain::lang::parse {

util::Result<std::unique_ptr<ast::IfExpression>> parse_if(lex::Lexer& lexer, ast::Scope& scope) {
    const auto token = lexer.next();
    if (token.kind != lex::TokenKind::If) {
        return ERR_PTR(err::SyntaxError, lexer, token.location,
                       "expected 'if'; this is an internal error");
    }

    auto condition = parse_any_expression(lexer, scope);
    FORWARD_ERROR(condition);

    if (auto token = lexer.peek(); token.kind != lex::TokenKind::LCurlyBracket) {
        return ERR_PTR(err::SyntaxError, lexer, token.location, "expected '{' after if condition");
    }
    auto then = parse_block(lexer, scope);
    FORWARD_ERROR(then);

    if (auto token = lexer.peek(); token.kind == lex::TokenKind::Else) {
        lexer.next();  // Consume the `else` token

        if (auto token = lexer.peek(); token.kind != lex::TokenKind::LCurlyBracket) {
            return ERR_PTR(err::SyntaxError, lexer, token.location, "expected '{' after if else");
        }

        auto else_ = parse_block(lexer, scope);
        FORWARD_ERROR(else_);

        return std::make_unique<ast::IfExpression>(
            std::move(condition).value(), std::move(then).value(), std::move(else_).value());
    }

    return std::make_unique<ast::IfExpression>(std::move(condition).value(),
                                               std::move(then).value());
}

}  // namespace rain::lang::parse
