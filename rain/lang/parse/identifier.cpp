#include "rain/lang/ast/expr/identifier.hpp"

#include <memory>

#include "rain/lang/ast/scope/scope.hpp"
#include "rain/lang/err/syntax.hpp"
#include "rain/lang/lex/lexer.hpp"
#include "rain/util/result.hpp"

namespace rain::lang::parse {

util::Result<std::unique_ptr<ast::IdentifierExpression>> parse_identifier(lex::Lexer& lexer,
                                                                          ast::Scope& scope) {
    const auto token = lexer.next();
    if (token.kind != lex::TokenKind::Identifier) {
        return ERR_PTR(err::SyntaxError, lexer, token.location,
                       "expected identifier; this is an internal error");
    }

    return std::make_unique<ast::IdentifierExpression>(token.text());
}

}  // namespace rain::lang::parse
