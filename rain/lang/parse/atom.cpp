#include "absl/strings/str_cat.h"
#include "rain/lang/ast/expr/expression.hpp"
#include "rain/lang/ast/scope/scope.hpp"
#include "rain/lang/err/syntax.hpp"
#include "rain/lang/lex/lexer.hpp"
#include "rain/lang/parse/all.hpp"
#include "rain/util/result.hpp"

namespace rain::lang::parse {

util::Result<std::unique_ptr<ast::Expression>> parse_atom(lex::Lexer& lexer, ast::Scope& scope) {
    std::unique_ptr<ast::Expression> expression;

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
            // case lex::TokenKind::False:
            //     lexer.next();
            //     expression = ast::BooleanExpression::alloc(false, token.location);
            //     break;

            // case lex::TokenKind::True:
            //     lexer.next();
            //     expression = ast::BooleanExpression::alloc(true, token.location);
            //     break;

        case lex::TokenKind::Integer: {
            auto result = parse_integer(lexer, scope);
            FORWARD_ERROR(result);
            expression = std::move(result).value();
            break;
        }

            // case lex::TokenKind::Float:
            //     expression = parse_float(lexer);
            //     break;

        case lex::TokenKind::Identifier: {
            auto result = parse_identifier(lexer, scope);
            FORWARD_ERROR(result);
            expression = std::move(result).value();
            break;
        }

        case lex::TokenKind::Self: {
            lexer.next();  // Consume the `self` token
            expression = std::make_unique<ast::IdentifierExpression>(token.text());
            break;
        }

            // case TokenKind::LRoundBracket:
            //     expression = parse_parenthesis(lexer);
            //     break;

        case lex::TokenKind::LCurlyBracket: {
            auto result = parse_block(lexer, scope);
            FORWARD_ERROR(result);
            expression = std::move(result).value();
            break;
        }

            // case TokenKind::If:
            //     expression = parse_if(lexer);
            //     break;

        case lex::TokenKind::Fn: {
            auto result = parse_function(lexer, scope);
            FORWARD_ERROR(result);
            expression = std::move(result).value();
            break;
        }

            // case lex::TokenKind::Hash:
            //     expression = parse_exec(lexer);
            //     break;

            // case lex::TokenKind::EndOfFile:
            //     return ERR_PTR(err::SyntaxError, lexer, token.location, "unexpected end of
            //     file");

        default:
            return ERR_PTR(
                err::SyntaxError, lexer, token.location,
                absl::StrCat("unexpected token '", token.text(), "' while parsing expression"));
    }

    auto next_token = lexer.peek();
    while (next_token.kind == lex::TokenKind::Period ||
           next_token.kind == lex::TokenKind::LRoundBracket) {
        if (next_token.kind == lex::TokenKind::Period) {
            auto result = parse_member(lexer, scope, std::move(expression));
            FORWARD_ERROR(result);

            expression = std::move(result).value();
            next_token = lexer.peek();
            continue;
        }

        if (next_token.kind == lex::TokenKind::LRoundBracket) {
            auto result = parse_call(lexer, scope, std::move(expression));
            FORWARD_ERROR(result);

            expression = std::move(result).value();
            next_token = lexer.peek();
            continue;
        }
    }

    return expression;
}

}  // namespace rain::lang::parse
