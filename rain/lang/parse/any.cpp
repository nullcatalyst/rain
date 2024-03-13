#include <utility>

#include "absl/strings/str_cat.h"
#include "rain/lang/ast/expr/expression.hpp"
#include "rain/lang/parse/all.hpp"

namespace rain::lang::parse {

util::Result<std::unique_ptr<ast::Expression>> parse_any_expression(lex::Lexer& lexer,
                                                                    Scope&      scope) {
    const auto token = lexer.peek();
    switch (token.kind) {
        case lex::TokenKind::Integer:
            return parse_integer(lexer, scope);

        case lex::TokenKind::LBracket:
            return parse_block(lexer, scope);

        default:
            return ERR_PTR(err::SyntaxError, lexer, token.location,
                           absl::StrCat("unexpected token \"", token.text(), "\""));
    }

    std::unreachable();
}

}  // namespace rain::lang::parse
