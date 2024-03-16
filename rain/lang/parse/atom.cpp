#include "absl/strings/str_cat.h"
#include "rain/lang/parse/all.hpp"
#include "rain/util/result.hpp"

namespace rain::lang::parse {

util::Result<std::unique_ptr<ast::Expression>> parse_atom(Lexer& lexer, Scope& scope) {
    util::Result<std::unique_ptr<ast::Expression>> expression;

    // {
    //     const auto state       = lexer.save_state();
    //     auto       ctor_result = parse_ctor(lexer);
    //     if (ctor_result.has_value()) {
    //         return ctor_result;
    //     }
    //     lexer.restore_state(state);
    // }

    const auto token = lexer.peek();
    switch (token.kind) {
            // case TokenKind::False:
            //     lexer.next();
            //     expression = ast::BooleanExpression::alloc(false, token.location);
            //     break;

            // case TokenKind::True:
            //     lexer.next();
            //     expression = ast::BooleanExpression::alloc(true, token.location);
            //     break;

        case TokenKind::Integer:
            expression = parse_integer(lexer);
            break;

            // case TokenKind::Float:
            //     expression = parse_float(lexer);
            //     break;

            // case TokenKind::Identifier:
            //     expression = parse_identifier(lexer);
            //     break;

            // case TokenKind::LRoundBracket:
            //     expression = parse_parenthesis(lexer);
            //     break;

            // case TokenKind::LCurlyBracket:
            //     expression = parse_block(lexer);
            //     break;

            // case TokenKind::If:
            //     expression = parse_if(lexer);
            //     break;

            // case TokenKind::Fn:
            //     expression = parse_function(lexer);
            //     break;

            // case TokenKind::Hash:
            //     expression = parse_exec(lexer);
            //     break;

            // case TokenKind::Eof:
            //     return ERR_PTR(err::SyntaxError, lexer, token.location, "unexpected end of
            //     file");

        default:
            return ERR_PTR(
                err::SyntaxError, lexer, token.location,
                absl::StrCat("unexpected token '", token.text(), "' while parsing expression"));
    }
    FORWARD_ERROR(expression);

    auto next_token = lexer.peek();
    while (next_token.kind == TokenKind::Period || next_token.kind == TokenKind::LRoundBracket) {
        if (next_token.kind == TokenKind::Period) {
            expression = parse_member(lexer, std::move(expression).value());
            FORWARD_ERROR(expression);

            next_token = lexer.peek();
            continue;
        }

        if (next_token.kind == TokenKind::LRoundBracket) {
            expression = parse_call(lexer, std::move(expression).value());
            FORWARD_ERROR(expression);

            next_token = lexer.peek();
            continue;
        }
    }

    return expression;
}

}  // namespace rain::lang::parse
