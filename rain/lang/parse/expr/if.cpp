#include "rain/lang/ast/expr/if.hpp"

#include <memory>

#include "rain/lang/ast/expr/block.hpp"
#include "rain/lang/ast/scope/scope.hpp"
#include "rain/lang/err/syntax.hpp"
#include "rain/lang/lex/lexer.hpp"
#include "rain/util/result.hpp"

namespace rain::lang::parse {

util::Result<std::unique_ptr<ast::Expression>>      parse_any_expression(lex::Lexer& lexer,
                                                                         ast::Scope& scope);
util::Result<std::unique_ptr<ast::BlockExpression>> parse_block(lex::Lexer& lexer,
                                                                ast::Scope& scope);

util::Result<std::unique_ptr<ast::IfExpression>> parse_if(lex::Lexer& lexer, ast::Scope& scope) {
    const auto if_token = lexer.next();
    IF_DEBUG {
        if (if_token.kind != lex::TokenKind::If) {
            return ERR_PTR(err::SyntaxError, if_token.location,
                           "expected 'if'; this is an internal error");
        }
    }

    auto condition = parse_any_expression(lexer, scope);
    FORWARD_ERROR(condition);

    if (auto token = lexer.peek(); token.kind != lex::TokenKind::LCurlyBracket) {
        return ERR_PTR(err::SyntaxError, token.location, "expected '{' after if condition");
    }
    auto then = parse_block(lexer, scope);
    FORWARD_ERROR(then);

    auto else_token = lexer.peek();
    if (else_token.kind == lex::TokenKind::Else) {
        lexer.next();  // Consume the `else` token

        auto if_or_lbracket_token = lexer.peek();
        if (if_or_lbracket_token.kind != lex::TokenKind::If &&
            if_or_lbracket_token.kind != lex::TokenKind::LCurlyBracket) {
            return ERR_PTR(err::SyntaxError, if_or_lbracket_token.location,
                           "expected 'if' or '{' after else");
        }

        if (if_or_lbracket_token.kind == lex::TokenKind::If) {
            auto else_implicit_block = std::make_unique<ast::BlockExpression>(scope);

            auto else_if_result = parse_if(lexer, else_implicit_block->scope());
            FORWARD_ERROR(else_if_result);

            auto else_if = std::move(else_if_result).value();
            else_implicit_block->set_location(else_if->location());
            else_implicit_block->add_expression(std::move(else_if));

            return std::make_unique<ast::IfExpression>(
                std::move(condition).value(), std::move(then).value(),
                std::move(else_implicit_block), if_token.location, else_token.location);
        }

        auto else_ = parse_block(lexer, scope);
        FORWARD_ERROR(else_);

        return std::make_unique<ast::IfExpression>(
            std::move(condition).value(), std::move(then).value(), std::move(else_).value(),
            if_token.location, else_token.location);
    }

    return std::make_unique<ast::IfExpression>(std::move(condition).value(),
                                               std::move(then).value(), if_token.location);
}

}  // namespace rain::lang::parse
