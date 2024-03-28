#include "rain/lang/ast/expr/let.hpp"

#include <memory>

#include "rain/lang/ast/scope/scope.hpp"
#include "rain/lang/err/syntax.hpp"
#include "rain/lang/lex/lexer.hpp"
#include "rain/lang/parse/all.hpp"
#include "rain/util/result.hpp"

namespace rain::lang::parse {

util::Result<std::unique_ptr<ast::LetExpression>> parse_let(lex::Lexer& lexer, ast::Scope& scope) {
    const auto let_token = lexer.next();
    if (let_token.kind != lex::TokenKind::Let) {
        return ERR_PTR(err::SyntaxError, let_token.location,
                       "expected 'let'; this is an internal error");
    }

    const auto name_token = lexer.next();
    if (name_token.kind != lex::TokenKind::Identifier) {
        return ERR_PTR(err::SyntaxError, name_token.location,
                       "expected identifier for variable name");
    }

    if (const auto eq_token = lexer.next(); eq_token.kind != lex::TokenKind::Equal) {
        return ERR_PTR(err::SyntaxError, eq_token.location,
                       "expected '=' between variable name and initial value");
    }

    auto value = parse_any_expression(lexer, scope);
    FORWARD_ERROR(value);

    return std::make_unique<ast::LetExpression>(name_token.text(), std::move(value).value(),
                                                let_token.location, name_token.location);
}

}  // namespace rain::lang::parse
