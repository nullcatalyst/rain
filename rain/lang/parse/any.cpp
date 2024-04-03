#include <utility>

#include "absl/strings/str_cat.h"
#include "rain/lang/ast/expr/expression.hpp"
#include "rain/lang/err/syntax.hpp"
#include "rain/lang/parse/all.hpp"
#include "rain/util/unreachable.hpp"

namespace rain::lang::parse {

util::Result<std::unique_ptr<ast::Expression>> parse_any_expression(lex::Lexer& lexer,
                                                                    ast::Scope& scope) {
    auto lhs = parse_atom(lexer, scope);
    FORWARD_ERROR(lhs);

    return parse_rhs(lexer, scope, std::move(lhs).value());
}

util::Result<std::unique_ptr<ast::Type>> parse_any_type(lex::Lexer& lexer, ast::Scope& scope) {
    const auto token = lexer.peek();
    switch (token.kind) {
        case lex::TokenKind::Struct:
            return parse_struct_type(lexer, scope);

        case lex::TokenKind::LSquareBracket:
            return parse_array_type(lexer, scope);

        case lex::TokenKind::Question:
            return parse_optional_type(lexer, scope);

        case lex::TokenKind::Identifier:
            lexer.next();  // Consume the identifier token
            return std::make_unique<ast::UnresolvedType>(token.text(), token.location);

        default:
            return ERR_PTR(err::SyntaxError, token.location,
                           absl::StrCat("unexpected token: ", token.text()));
    }

    util::unreachable();
}

}  // namespace rain::lang::parse
