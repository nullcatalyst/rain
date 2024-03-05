#include "rain/ast/type/all.hpp"
#include "rain/err/all.hpp"
#include "rain/lang/lexer.hpp"
#include "rain/lang/parse+common.hpp"
#include "rain/util/result.hpp"

namespace rain::lang {

util::Result<std::shared_ptr<ast::StructType>> parse_struct(Lexer& lexer) {
    if (const auto struct_token = lexer.next(); struct_token.kind != TokenKind::Struct) {
        return ERR_PTR(err::SyntaxError, lexer, struct_token.location,
                       "expected keyword 'struct'; this is an internal error");
    }

    std::optional<util::String> struct_name;
    auto                        next_token = lexer.next();

    // The struct name is optional
    if (next_token.kind == TokenKind::Identifier) {
        struct_name = next_token.location.substr();
        next_token  = lexer.next();
    }

    if (next_token.kind != TokenKind::LCurlyBracket) {
        return ERR_PTR(err::SyntaxError, lexer, next_token.location,
                       "expected '{' after struct name");
    }

    std::vector<ast::StructTypeFieldData> struct_fields;
    for (;;) {
        // Handle the struct fields
        auto next_token = lexer.next();

        switch (next_token.kind) {
            case TokenKind::RCurlyBracket:
                return ast::StructType::alloc(std::move(struct_name), std::move(struct_fields));

            case TokenKind::Identifier:
                // Found a field name
                break;

            default:
                return ERR_PTR(err::SyntaxError, lexer, next_token.location,
                               "expected identifier for struct field name");
        }

        const auto field_name = next_token.location.substr();

        next_token = lexer.next();
        if (next_token.kind != TokenKind::Colon) {
            return ERR_PTR(err::SyntaxError, lexer, next_token.location,
                           "expected ':' after struct field name");
        }

        auto field_type = parse_whole_type(lexer);
        FORWARD_ERROR(field_type);

        struct_fields.emplace_back(ast::StructTypeFieldData{
            .name = field_name,
            .type = std::move(field_type).value(),
        });

        next_token = lexer.next();
        if (next_token.kind != TokenKind::Semicolon) {
            return ERR_PTR(err::SyntaxError, lexer, next_token.location,
                           "expected ';' after struct field definition");
        }
    }

    __builtin_unreachable();
}

util::Result<std::shared_ptr<ast::InterfaceType>> parse_interface(Lexer& lexer) {
    if (const auto interface_token = lexer.next(); interface_token.kind != TokenKind::Interface) {
        return ERR_PTR(err::SyntaxError, lexer, interface_token.location,
                       "expected keyword 'interface'; this is an internal error");
    }

    std::optional<util::String> interface_name;
    auto                        next_token = lexer.next();

    // The struct name is optional
    if (next_token.kind == TokenKind::Identifier) {
        interface_name = next_token.location.substr();
        next_token     = lexer.next();
    }

    if (next_token.kind != TokenKind::LCurlyBracket) {
        return ERR_PTR(err::SyntaxError, lexer, next_token.location,
                       "expected '{' after interface name");
    }

    // TODO: Parse the interface methods
    std::abort();
}

util::Result<ast::TypePtr> parse_whole_type(Lexer& lexer) {
    const auto token = lexer.peek();
    switch (token.kind) {
        case TokenKind::Struct:
            return parse_struct(lexer);

        case TokenKind::Interface:
            return parse_interface(lexer);

        case TokenKind::Identifier:
            lexer.next();  // Consume the identifier token
            return ast::UnresolvedType::alloc(token.location.substr());

        default:
            return ERR_PTR(err::SyntaxError, lexer, token.location,
                           "unexpected token while trying to parse a type");
    }
}

}  // namespace rain::lang
