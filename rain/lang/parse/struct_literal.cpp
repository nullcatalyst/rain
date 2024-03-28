#include <memory>

#include "rain/lang/err/syntax.hpp"
#include "rain/lang/lex/lexer.hpp"
#include "rain/lang/parse/all.hpp"
#include "rain/lang/parse/list.hpp"
#include "rain/util/result.hpp"

namespace rain::lang::parse {

util::Result<std::unique_ptr<ast::StructLiteralExpression>> parse_struct_literal(
    lex::Lexer& lexer, ast::Scope& scope) {
    const auto first_token = lexer.peek();

    auto type = parse_any_type(lexer, scope);
    FORWARD_ERROR(type);

    const auto lbracket_token = lexer.next();
    if (lbracket_token.kind != lex::TokenKind::LCurlyBracket) {
        return ERR_PTR(err::SyntaxError, lbracket_token.location,
                       "expected '{' after struct type in struct literal");
    }

    std::vector<ast::StructLiteralField> fields;
    auto                                 result = parse_list(
        lexer, lex::TokenKind::Comma, lex::TokenKind::RCurlyBracket,
        [&](lex::Lexer& lexer) -> util::Result<void> {
            const auto field_name = lexer.next();
            if (field_name.kind != lex::TokenKind::Identifier) {
                return ERR_PTR(err::SyntaxError, field_name.location,
                                                               "expected identifier for struct literal field name");
            }

            if (const auto colon_token = lexer.next(); colon_token.kind != lex::TokenKind::Colon) {
                return ERR_PTR(err::SyntaxError, colon_token.location,
                                                               "expected ':' between field name and field value");
            }

            auto field_value = parse_any_expression(lexer, scope);
            FORWARD_ERROR(field_value);

            fields.emplace_back(ast::StructLiteralField{
                                                .name  = field_name.text(),
                                                .value = std::move(field_value).value(),
                                                .index = -1,
            });
            return {};
        },
        [](lex::Lexer& lexer, lex::Token token) -> util::Result<void> {
            return ERR_PTR(err::SyntaxError, token.location,
                                                           "expected ',' or '}' after struct field");
        });
    FORWARD_ERROR(result);

    const auto rbracket_token = lexer.next();  // Consume the '}'

    return std::make_unique<ast::StructLiteralExpression>(
        std::move(type).value(), std::move(fields),
        first_token.location.merge(rbracket_token.location));
}

}  // namespace rain::lang::parse
