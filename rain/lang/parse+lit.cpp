#include "rain/ast/expr/all.hpp"
#include "rain/err/all.hpp"
#include "rain/lang/lexer.hpp"
#include "rain/lang/parse+common.hpp"
#include "rain/util/result.hpp"

namespace rain::lang {

util::Result<std::unique_ptr<ast::IntegerExpression>> parse_integer(Lexer& lexer) {
    const auto token = lexer.next();
    if (token.kind != TokenKind::Integer) {
        return ERR_PTR(err::SyntaxError, lexer, token.location, "expected integer literal");
    }

    constexpr uint64_t MAX   = std::numeric_limits<uint64_t>::max() / 10;
    uint64_t           value = 0;

    for (const char c : token.location.substr()) {
        if (value > MAX) {
            // Multiplying by 10 will overflow
            return ERR_PTR(err::SyntaxError, lexer, token.location, "integer literal is too large");
        }
        value *= 10;

        if (value > std::numeric_limits<uint64_t>::max() - (c - '0')) {
            // Adding the next digit will overflow
            return ERR_PTR(err::SyntaxError, lexer, token.location, "integer literal is too large");
        }
        value += c - '0';
    }

    return ast::IntegerExpression::alloc(value);
}

util::Result<std::unique_ptr<ast::FloatExpression>> parse_float(Lexer& lexer) {
    const auto token = lexer.next();
    if (token.kind != TokenKind::Float) {
        return ERR_PTR(err::SyntaxError, lexer, token.location, "expected floating point literal");
    }

    double value = 0.0;
    if (!absl::SimpleAtod(token.location.substr(), &value)) {
        return ERR_PTR(err::SyntaxError, lexer, token.location, "invalid floating point literal");
    }

    return ast::FloatExpression::alloc(value);
}

util::Result<std::unique_ptr<ast::IdentifierExpression>> parse_identifier(Lexer& lexer) {
    const auto token = lexer.next();
    if (token.kind != TokenKind::Identifier) {
        return ERR_PTR(err::SyntaxError, lexer, token.location, "expected identifier");
    }

    return ast::IdentifierExpression::alloc(token.location.substr());
}

util::Result<std::unique_ptr<ast::ParenthesisExpression>> parse_parenthesis(Lexer& lexer) {
    const auto lround_token = lexer.next();
    if (lround_token.kind != TokenKind::LRoundBracket) {
        return ERR_PTR(err::SyntaxError, lexer, lround_token.location, "expected '('");
    }

    auto expr = parse_whole_expression(lexer);
    FORWARD_ERROR(expr);

    const auto rround_token = lexer.next();
    if (rround_token.kind != TokenKind::RRoundBracket) {
        return ERR_PTR(err::SyntaxError, lexer, rround_token.location, "expected ')'");
    }

    return ast::ParenthesisExpression::alloc(std::move(expr).value());
}

util::Result<std::unique_ptr<ast::MemberExpression>> parse_member(Lexer&             lexer,
                                                                  ast::ExpressionPtr owner) {
    const auto period_token = lexer.next();
    if (period_token.kind != TokenKind::Period) {
        return ERR_PTR(err::SyntaxError, lexer, period_token.location, "expected '.'");
    }

    const auto member_token = lexer.next();
    if (member_token.kind != TokenKind::Identifier) {
        return ERR_PTR(err::SyntaxError, lexer, member_token.location,
                       "expected identifier after '.'");
    }

    return ast::MemberExpression::alloc(std::move(owner), member_token.location.substr());
}

util::Result<std::unique_ptr<ast::CallExpression>> parse_call(Lexer&             lexer,
                                                              ast::ExpressionPtr callee) {
    const auto lbracket_token = lexer.next();
    if (lbracket_token.kind != TokenKind::LRoundBracket) {
        return ERR_PTR(err::SyntaxError, lexer, lbracket_token.location, "expected '('");
    }

    std::vector<ast::ExpressionPtr> arguments;
    {
        // Handle the first argument; it's special because it doesn't have a preceding comma
        const auto next_token = lexer.peek();
        if (next_token.kind == TokenKind::RRoundBracket) {
            lexer.next();  // Consume the ')' token
            return ast::CallExpression::alloc(std::move(callee), std::move(arguments));
        }

        auto argument = parse_whole_expression(lexer);
        FORWARD_ERROR(argument);

        arguments.emplace_back(std::move(argument).value());
    }

    for (;;) {
        // Handle the rest of the arguments
        auto next_token = lexer.peek();
        switch (next_token.kind) {
            case TokenKind::Comma:
                lexer.next();  // Consume the ',' token

                if (lexer.peek().kind == TokenKind::RRoundBracket) {
                    lexer.next();  // Consume the ')' token
                    return ast::CallExpression::alloc(std::move(callee), std::move(arguments));
                }
                break;

            case TokenKind::RRoundBracket:
                lexer.next();  // Consume the ')' token
                return ast::CallExpression::alloc(std::move(callee), std::move(arguments));

            default:
                return ERR_PTR(err::SyntaxError, lexer, next_token.location, "expected ',' or ')'");
        }

        auto argument = parse_whole_expression(lexer);
        FORWARD_ERROR(argument);

        arguments.emplace_back(std::move(argument).value());
    }

    __builtin_unreachable();
}

util::Result<std::unique_ptr<ast::ExecExpression>> parse_exec(Lexer& lexer) {
    const auto token = lexer.next();
    if (token.kind != TokenKind::Hash) {
        return ERR_PTR(err::SyntaxError, lexer, token.location, "expected '#'");
    }

    auto expr = parse_atom(lexer);
    FORWARD_ERROR(expr);

    return ast::ExecExpression::alloc(std::move(expr).value());
}

util::Result<ast::ExpressionPtr> parse_atom(Lexer& lexer) {
    util::Result<ast::ExpressionPtr> expression;

    const auto token = lexer.peek();
    switch (token.kind) {
        case TokenKind::Integer:
            expression = parse_integer(lexer);
            break;

        case TokenKind::Float:
            expression = parse_float(lexer);
            break;

        case TokenKind::Identifier:
            expression = parse_identifier(lexer);
            break;

        case TokenKind::LRoundBracket:
            expression = parse_parenthesis(lexer);
            break;

        case TokenKind::LCurlyBracket:
            expression = parse_block(lexer);
            break;

        case TokenKind::If:
            expression = parse_if(lexer);
            break;

        case TokenKind::Fn:
            expression = parse_function(lexer);
            break;

        case TokenKind::Hash:
            expression = parse_exec(lexer);
            break;

        case TokenKind::Eof:
            return ERR_PTR(err::SyntaxError, lexer, token.location, "unexpected end of file");

        default:
            return ERR_PTR(err::SyntaxError, lexer, token.location,
                           "unexpected token while parsing expression");
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

}  // namespace rain::lang
