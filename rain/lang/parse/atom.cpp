#include "absl/strings/str_cat.h"
#include "rain/lang/ast/expr/expression.hpp"
#include "rain/lang/ast/scope/scope.hpp"
#include "rain/lang/err/syntax.hpp"
#include "rain/lang/lex/lexer.hpp"
#include "rain/lang/parse/all.hpp"
#include "rain/util/result.hpp"

namespace rain::lang::parse {

namespace {

util::Result<std::unique_ptr<ast::Expression>> parse_standalone_atom(lex::Lexer& lexer,
                                                                     ast::Scope& scope) {
    const auto token = lexer.peek();
    switch (token.kind) {
        case lex::TokenKind::False: {
            lexer.next();  // Consume the `false` token
            return std::make_unique<ast::BooleanExpression>(false, token.location);
        }

        case lex::TokenKind::True: {
            lexer.next();  // Consume the `true` token
            return std::make_unique<ast::BooleanExpression>(true, token.location);
        }

        case lex::TokenKind::Integer:
            return parse_integer(lexer, scope);

        case lex::TokenKind::Float:
            return parse_float(lexer, scope);

        case lex::TokenKind::Identifier:
            return parse_identifier(lexer, scope);

        case lex::TokenKind::Self: {
            lexer.next();  // Consume the `self` token
            return std::make_unique<ast::IdentifierExpression>(token.text(), token.location);
        }

        case lex::TokenKind::Let:
            return parse_let(lexer, scope);

        case lex::TokenKind::LRoundBracket:
            return parse_parenthesis(lexer, scope);

        case lex::TokenKind::LCurlyBracket:
            return parse_block(lexer, scope);

        case lex::TokenKind::If:
            return parse_if(lexer, scope);

        case lex::TokenKind::While:
            return parse_while(lexer, scope);

        case lex::TokenKind::Fn:
            return parse_function(lexer, scope);

        case lex::TokenKind::Hash:
            return parse_compile_time(lexer, scope);

        case lex::TokenKind::Extern:
            return parse_extern(lexer, scope);

        default:
            return parse_unary_operator(lexer, scope);
    }
}

}  // namespace

util::Result<std::unique_ptr<ast::Expression>> parse_atom(lex::Lexer& lexer, ast::Scope& scope) {
    std::unique_ptr<ast::Expression> expression;

    {
        const auto state       = lexer.save_state();
        auto       ctor_result = parse_struct_literal(lexer, scope);
        if (ctor_result.has_value()) {
            expression = std::move(ctor_result).value();
        } else {
            lexer.restore_state(state);

            auto result = parse_standalone_atom(lexer, scope);
            FORWARD_ERROR(result);
            expression = std::move(result).value();
        }
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
