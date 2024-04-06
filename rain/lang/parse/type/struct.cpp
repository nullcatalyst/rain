#include "rain/lang/ast/type/struct.hpp"

#include <memory>

#include "rain/lang/ast/scope/scope.hpp"
#include "rain/lang/err/syntax.hpp"
#include "rain/lang/lex/lexer.hpp"
#include "rain/lang/parse/util/list.hpp"
#include "rain/util/result.hpp"

namespace rain::lang::parse {

util::Result<absl::Nonnull<ast::Type*>> parse_any_type(lex::Lexer& lexer, ast::Scope& scope);

util::Result<absl::Nonnull<ast::StructType*>> parse_struct_type(lex::Lexer& lexer,
                                                                ast::Scope& scope) {
    const auto struct_token = lexer.next();
    IF_DEBUG {
        if (struct_token.kind != lex::TokenKind::Struct) {
            return ERR_PTR(err::SyntaxError, struct_token.location,
                           "expected keyword 'struct'; this is an internal error");
        }
    }

    auto name_token = lexer.next();
    if (name_token.kind != lex::TokenKind::Identifier) {
        return ERR_PTR(err::SyntaxError, name_token.location,
                       "expected identifier for struct name");
    }
    std::string_view struct_name = name_token.text();

    if (auto lbracket_token = lexer.next(); lbracket_token.kind != lex::TokenKind::LCurlyBracket) {
        return ERR_PTR(err::SyntaxError, lbracket_token.location,
                       "expected '{' after interface name");
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

            fields.emplace_back(field_name.text(), std::move(field_type).value());
            return {};
        },
        [](lex::Lexer& lexer, lex::Token token) -> util::Result<void> {
            return ERR_PTR(err::SyntaxError, token.location,
                                                    "expected ',' or '}' after struct field");
        });
    FORWARD_ERROR(result);

    const auto rbracket_token = lexer.next();  // Consume the '}'

    return static_cast<ast::StructType*>(scope.add_type(
        struct_name,
        std::make_unique<ast::StructType>(struct_name, std::move(fields),
                                          struct_token.location.merge(rbracket_token.location))));
}

}  // namespace rain::lang::parse
