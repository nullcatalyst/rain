#include <memory>

#include "absl/base/nullability.h"
#include "rain/lang/ast/expr/array_literal.hpp"
#include "rain/lang/ast/expr/slice_literal.hpp"
#include "rain/lang/ast/expr/struct_literal.hpp"
#include "rain/lang/ast/type/type.hpp"
#include "rain/lang/err/syntax.hpp"
#include "rain/lang/lex/lexer.hpp"
#include "rain/lang/parse/util/list.hpp"
#include "rain/util/result.hpp"

namespace rain::lang::parse {

util::Result<std::unique_ptr<ast::Expression>> parse_any_expression(lex::Lexer& lexer,
                                                                    ast::Scope& scope);

util::Result<absl::Nonnull<ast::Type*>> parse_any_type(lex::Lexer& lexer, ast::Scope& scope);

namespace {

util::Result<std::unique_ptr<ast::Expression>> _parse_array_or_slice_literal(
    lex::Lexer& lexer, ast::Scope& scope, lex::Token first_token, absl::Nonnull<ast::Type*> type) {
    std::vector<std::unique_ptr<ast::Expression>> elements;
    auto                                          result = parse_list(
        lexer, lex::TokenKind::Comma, lex::TokenKind::RCurlyBracket,
        [&](lex::Lexer& lexer) -> util::Result<void> {
            auto element_value = parse_any_expression(lexer, scope);
            FORWARD_ERROR(element_value);

            elements.emplace_back(std::move(element_value).value());
            return {};
        },
        [](lex::Lexer& lexer, lex::Token token) -> util::Result<void> {
            return ERR_PTR(err::SyntaxError, token.location,
                                                                    "expected ',' or '}' after array element");
        });
    FORWARD_ERROR(result);

    const auto rbracket_token = lexer.next();  // Consume the '}'

    if (type->kind() == serial::TypeKind::Slice) {
        return std::make_unique<ast::SliceLiteralExpression>(
            type, std::move(elements), first_token.location.merge(rbracket_token.location));
    }
    return std::make_unique<ast::ArrayLiteralExpression>(
        type, std::move(elements), first_token.location.merge(rbracket_token.location));
}

util::Result<std::unique_ptr<ast::Expression>> _parse_struct_literal(
    lex::Lexer& lexer, ast::Scope& scope, lex::Token first_token, absl::Nonnull<ast::Type*> type) {
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
        type, std::move(fields), first_token.location.merge(rbracket_token.location));
}

}  // namespace

util::Result<std::unique_ptr<ast::Expression>> parse_struct_literal(lex::Lexer& lexer,
                                                                    ast::Scope& scope) {
    const auto first_token = lexer.peek();

    auto type_result = parse_any_type(lexer, scope);
    FORWARD_ERROR(type_result);
    absl::Nonnull<ast::Type*> type = std::move(type_result).value();

    const auto lbracket_token = lexer.next();
    if (lbracket_token.kind != lex::TokenKind::LCurlyBracket) {
        if (type->kind() == serial::TypeKind::Array) {
            return ERR_PTR(err::SyntaxError, lbracket_token.location,
                           "expected '{' after array type in array literal");
        }

        return ERR_PTR(err::SyntaxError, lbracket_token.location,
                       "expected '{' after struct type in struct literal");
    }

    if (type->kind() == serial::TypeKind::Array || type->kind() == serial::TypeKind::Slice) {
        return _parse_array_or_slice_literal(lexer, scope, first_token, type);
    }
    return _parse_struct_literal(lexer, scope, first_token, type);
}

}  // namespace rain::lang::parse
