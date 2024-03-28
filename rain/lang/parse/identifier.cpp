#include "rain/lang/ast/expr/identifier.hpp"

#include <memory>

#include "rain/lang/ast/scope/scope.hpp"
#include "rain/lang/err/syntax.hpp"
#include "rain/lang/lex/lexer.hpp"
#include "rain/util/result.hpp"

namespace rain::lang::parse {

util::Result<std::unique_ptr<ast::IdentifierExpression>> parse_identifier(lex::Lexer& lexer,
                                                                          ast::Scope& scope) {
    const auto identifier_token = lexer.next();
    if (identifier_token.kind != lex::TokenKind::Identifier) {
        return ERR_PTR(err::SyntaxError, identifier_token.location,
                       "expected identifier; this is an internal error");
    }

    return std::make_unique<ast::IdentifierExpression>(identifier_token.text(),
                                                       identifier_token.location);
}

}  // namespace rain::lang::parse
