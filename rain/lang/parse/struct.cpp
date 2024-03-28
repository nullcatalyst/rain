#include <memory>

#include "rain/lang/err/syntax.hpp"
#include "rain/lang/lex/lexer.hpp"
#include "rain/lang/parse/all.hpp"
#include "rain/lang/parse/list.hpp"
#include "rain/util/result.hpp"

namespace rain::lang::parse {

util::Result<std::unique_ptr<ast::StructType>> parse_struct(lex::Lexer& lexer, ast::Scope& scope) {
    if (const auto struct_token = lexer.next(); struct_token.kind != lex::TokenKind::Struct) {
        // This function should only be called if we already know the next token starts a struct.
        return ERR_PTR(err::SyntaxError, struct_token.location,
                       "expected keyword 'struct'; this is an internal error");
    }

    std::optional<std::string_view> struct_name;
    auto                            next_token = lexer.next();
    // if (next_token.kind == lex::TokenKind::Identifier) {
    //     struct_name = next_token.text();
    //     next_token  = lexer.next();
    // }
    {
        // Temporarily make struct names required, while we determine the best semantics for unnamed
        // structs.
        if (next_token.kind != lex::TokenKind::Identifier) {
            return ERR_PTR(err::SyntaxError, next_token.location,
                           "expected identifier for struct name");
        }
        struct_name = next_token.text();
        next_token  = lexer.next();
    }

    if (next_token.kind != lex::TokenKind::LCurlyBracket) {
        return ERR_PTR(err::SyntaxError, next_token.location, "expected '{' after struct name");
    }

    std::vector<ast::StructField> fields;
    auto                          result = parse_list(
        lexer, lex::TokenKind::Comma, lex::TokenKind::RCurlyBracket,
        [&](lex::Lexer& lexer) -> util::Result<void> {
            const auto field_name = lexer.next();
            if (field_name.kind != lex::TokenKind::Identifier) {
                return ERR_PTR(err::SyntaxError, field_name.location,
                                                        "expected identifier for struct field name");
            }

            if (const auto colon_token = lexer.next(); colon_token.kind != lex::TokenKind::Colon) {
                return ERR_PTR(err::SyntaxError, colon_token.location,
                                                        "expected ':' between field name and field type");
            }

            auto field_type = parse_any_type(lexer, scope);
            FORWARD_ERROR(field_type);

            fields.emplace_back(ast::StructField{
                                         .name = field_name.text(),
                                         .type = std::move(field_type).value(),
            });
            return {};
        },
        [](lex::Lexer& lexer, lex::Token token) -> util::Result<void> {
            return ERR_PTR(err::SyntaxError, token.location,
                                                    "expected ',' or '}' after struct field");
        });
    FORWARD_ERROR(result);

    return std::make_unique<ast::StructType>(std::move(struct_name), std::move(fields));
}

}  // namespace rain::lang::parse
