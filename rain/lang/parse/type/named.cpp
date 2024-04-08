#include <memory>

#include "rain/lang/ast/scope/scope.hpp"
#include "rain/lang/err/syntax.hpp"
#include "rain/lang/lex/lexer.hpp"
#include "rain/util/result.hpp"

namespace rain::lang::parse {

util::Result<absl::Nonnull<ast::Type*>> parse_named_type(lex::Lexer& lexer, ast::Scope& scope) {
    const auto name_token = lexer.next();
    IF_DEBUG {
        if (name_token.kind != lex::TokenKind::Identifier) {
            return ERR_PTR(err::SyntaxError, name_token.location,
                           "expected type name; this is an internal error");
        }
    }

    return scope.find_or_create_unresolved_named_type(name_token.text(), name_token.location);
}

}  // namespace rain::lang::parse
