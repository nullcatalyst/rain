#include "rain/lang/ast/expr/while.hpp"

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

util::Result<std::unique_ptr<ast::WhileExpression>> parse_while(lex::Lexer& lexer,
                                                                ast::Scope& scope) {
    const auto while_token = lexer.next();
    IF_DEBUG {
        if (while_token.kind != lex::TokenKind::While) {
            return ERR_PTR(err::SyntaxError, while_token.location,
                           "expected 'while'; this is an internal error");
        }
    }

    auto condition = parse_any_expression(lexer, scope);
    FORWARD_ERROR(condition);

    if (auto token = lexer.peek(); token.kind != lex::TokenKind::LCurlyBracket) {
        return ERR_PTR(err::SyntaxError, token.location, "expected '{' after while condition");
    }
    auto loop = parse_block(lexer, scope);
    FORWARD_ERROR(loop);

    if (auto else_token = lexer.peek(); else_token.kind == lex::TokenKind::Else) {
        lexer.next();  // Consume the `else` token

        if (auto token = lexer.peek(); token.kind != lex::TokenKind::LCurlyBracket) {
            return ERR_PTR(err::SyntaxError, token.location, "expected '{' after while else");
        }

        auto else_ = parse_block(lexer, scope);
        FORWARD_ERROR(else_);

        return std::make_unique<ast::WhileExpression>(
            std::move(condition).value(), std::move(loop).value(), std::move(else_).value(),
            while_token.location, else_token.location);
    }

    return std::make_unique<ast::WhileExpression>(std::move(condition).value(),
                                                  std::move(loop).value(), while_token.location);
}

}  // namespace rain::lang::parse
