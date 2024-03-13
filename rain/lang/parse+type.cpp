#include "rain/ast/type/all.hpp"
#include "rain/err/all.hpp"
#include "rain/lang/lexer.hpp"
#include "rain/lang/parse+common.hpp"
#include "rain/util/result.hpp"

namespace rain::lang {

util::Result<std::shared_ptr<ast::StructType>> parse_struct(Lexer& lexer) {
    const auto struct_token = lexer.next();
    if (struct_token.kind != TokenKind::Struct) {
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

    std::vector<ast::StructTypeField> fields;

#define PARSE_CTOR_FIELD()                                                                  \
    do {                                                                                    \
        const auto name_token = lexer.next();                                               \
        if (name_token.kind != TokenKind::Identifier) {                                     \
            return ERR_PTR(err::SyntaxError, lexer, name_token.location,                    \
                           "expected identifier for field name in constructor expression"); \
        }                                                                                   \
        auto name = name_token.location.substr();                                           \
                                                                                            \
        if (const auto colon_token = lexer.next(); colon_token.kind != TokenKind::Colon) {  \
            return ERR_PTR(err::SyntaxError, lexer, colon_token.location,                   \
                           "expected ':' between field name and initializer value");        \
        }                                                                                   \
                                                                                            \
        auto type = parse_whole_type(lexer);                                                \
        FORWARD_ERROR(type);                                                                \
                                                                                            \
        fields.emplace_back(ast::StructTypeField{                                           \
            .name = std::move(name),                                                        \
            .type = std::move(type).value(),                                                \
        });                                                                                 \
    } while (false)

    {
        // Trivial case: no struct fields.
        const auto next_token = lexer.peek();
        if (next_token.kind == TokenKind::RCurlyBracket) {
            lexer.next();  // Consume the '}' token
            return ast::StructType::alloc(struct_name, std::move(fields));
        }

        PARSE_CTOR_FIELD();
    }

    for (;;) {
        // Handle the rest of the arguments
        auto next_token = lexer.peek();
        switch (next_token.kind) {
            case TokenKind::Comma:
                lexer.next();  // Consume the ',' token

                if (lexer.peek().kind == TokenKind::RCurlyBracket) {
                    lexer.next();  // Consume the '}' token
                    return ast::StructType::alloc(struct_name, std::move(fields));
                }
                break;

            case TokenKind::RCurlyBracket:
                lexer.next();  // Consume the '}' token
                return ast::StructType::alloc(struct_name, std::move(fields));

            default:
                return ERR_PTR(err::SyntaxError, lexer, next_token.location, "expected ',' or '}'");
        }

        PARSE_CTOR_FIELD();
    }

    // __builtin_unreachable();

#undef PARSE_CTOR_FIELD
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
