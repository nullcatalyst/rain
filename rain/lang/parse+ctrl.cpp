#include "rain/ast/expr/all.hpp"
#include "rain/err/all.hpp"
#include "rain/lang/lexer.hpp"
#include "rain/lang/parse+common.hpp"
#include "rain/util/result.hpp"

namespace rain::lang {

util::Result<std::unique_ptr<ast::BlockExpression>> parse_block(Lexer& lexer) {
    const auto lbracket_token = lexer.next();
    if (lbracket_token.kind != TokenKind::LCurlyBracket) {
        return ERR_PTR(err::SyntaxError, lexer, lbracket_token.location, "expected '{'");
    }

    if (const auto next_token = lexer.peek(); next_token.kind == TokenKind::RCurlyBracket) {
        lexer.next();  // Consume the '}' token
        return ast::BlockExpression::alloc(std::vector<ast::StatementPtr>{}, nullptr,
                                           lbracket_token.location.merge(next_token.location));
    }

    std::vector<ast::StatementPtr> statements;
    for (;;) {
        auto expression_result = parse_whole_expression(lexer);
        FORWARD_ERROR(expression_result);
        auto expression = std::move(expression_result).value();

        auto next_token = lexer.peek();
        if (next_token.kind == TokenKind::Semicolon) {
            lexer.next();  // Consume the ';' token
            statements.emplace_back(ast::StatementExpression::alloc(std::move(expression)));

            next_token = lexer.peek();
            if (next_token.kind == TokenKind::RCurlyBracket) [[unlikely]] {
                lexer.next();  // Consume the '}' token
                return ast::BlockExpression::alloc(
                    std::move(statements), nullptr,
                    lbracket_token.location.merge(next_token.location));
            }

            continue;
        }
        if (next_token.kind == TokenKind::RCurlyBracket) {
            lexer.next();  // Consume the '}' token
            return ast::BlockExpression::alloc(std::move(statements), std::move(expression),
                                               lbracket_token.location.merge(next_token.location));
        }

        return ERR_PTR(err::SyntaxError, lexer, expression->location().empty_string_after(),
                       "unexpected token; statements must be separated by ';', or the current "
                       "block can be closed with '}'");
    }

    // __builtin_unreachable();
}

util::Result<std::unique_ptr<ast::IfExpression>> parse_if(Lexer& lexer) {
    const auto if_token = lexer.next();
    if (if_token.kind != TokenKind::If) {
        return ERR_PTR(err::SyntaxError, lexer, if_token.location,
                       "expected keyword 'if'; this is an internal error");
    }

    auto condition = parse_whole_expression(lexer);
    FORWARD_ERROR(condition);

    auto then = parse_block(lexer);
    FORWARD_ERROR(then);

    std::optional<ast::ExpressionPtr> opt_else_;

    lang::Location else_location;
    const auto     else_token = lexer.peek();
    if (else_token.kind == TokenKind::Else) {
        lexer.next();  // Consume the 'else' token

        auto else_ = parse_block(lexer);
        FORWARD_ERROR(else_);

        opt_else_     = std::move(else_).value();
        else_location = else_token.location;
    }

    return ast::IfExpression::alloc(std::move(condition).value(), std::move(then).value(),
                                    std::move(opt_else_), if_token.location, else_location);
}

util::Result<std::unique_ptr<ast::FunctionExpression>> parse_function(Lexer& lexer) {
    const auto function_token = lexer.next();
    if (function_token.kind != TokenKind::Fn) {
        return ERR_PTR(err::SyntaxError, lexer, function_token.location,
                       "expected keyword 'fn'; this is an internal error");
    }

    ast::TypePtr method_owner;
    {
        const auto state = lexer.save_state();

        auto type_result = parse_whole_type(lexer);
        if (const auto peek_token = lexer.peek(); peek_token.kind == TokenKind::Period) {
            lexer.next();  // Consume the '.' token

            // Ensure that the owning type parsed successfully.
            FORWARD_ERROR(type_result);

            method_owner = std::move(type_result).value();
        } else {
            // This is not a method, so restore the lexer state, so that any parsed tokens are
            // treated as a function name instead of a type name.
            lexer.restore_state(state);
        }
    }

    std::optional<util::String> fn_name;
    auto                        next_token = lexer.next();
    if (next_token.kind == TokenKind::Identifier) {
        fn_name    = next_token.location.substr();
        next_token = lexer.next();
    } else if (method_owner != nullptr) {
        return ERR_PTR(err::SyntaxError, lexer, next_token.location,
                       "expected function name after type name");
    }

    if (next_token.kind != TokenKind::LRoundBracket) {
        return ERR_PTR(err::SyntaxError, lexer, next_token.location,
                       "expected '(' before function arguments");
    }

    std::vector<ast::FunctionArgumentData> arguments;
    for (;;) {
        // Handle the function arguments
        auto next_token = lexer.next();
        if (next_token.kind == TokenKind::RRoundBracket) {
            break;
        }

        if (next_token.kind != TokenKind::Identifier) {
            return ERR_PTR(err::SyntaxError, lexer, next_token.location,
                           "expected identifier for function argument name");
        }

        const auto argument_name = next_token.location.substr();

        next_token = lexer.next();
        if (next_token.kind != TokenKind::Colon) {
            return ERR_PTR(err::SyntaxError, lexer, next_token.location,
                           "expected ':' after function argument name");
        }

        auto argument_type = parse_whole_type(lexer);
        FORWARD_ERROR(argument_type);

        arguments.emplace_back(ast::FunctionArgumentData{
            .name = argument_name,
            .type = std::move(argument_type).value(),
        });

        next_token = lexer.next();
        if (next_token.kind == TokenKind::RRoundBracket) {
            break;
        }

        if (next_token.kind != TokenKind::Comma) {
            return ERR_PTR(err::SyntaxError, lexer, next_token.location,
                           "expected ',' or ')' after function argument definition");
        }
    }

    std::optional<ast::TypePtr> return_type;
    next_token = lexer.peek();
    if (next_token.kind == TokenKind::RArrow) {
        lexer.next();  // Consume the '->' token

        auto return_type_result = parse_whole_type(lexer);
        FORWARD_ERROR(return_type_result);

        return_type = std::move(return_type_result).value();
    }

    auto body = parse_block(lexer);
    FORWARD_ERROR(body);

    return ast::FunctionExpression::alloc(std::move(fn_name), std::move(method_owner),
                                          std::move(return_type), std::move(arguments),
                                          std::move(body).value(), function_token.location);
}

util::Result<std::unique_ptr<ast::LetExpression>> parse_let(Lexer& lexer) {
    const auto let_token = lexer.next();
    if (let_token.kind != TokenKind::Let) {
        return ERR_PTR(err::SyntaxError, lexer, let_token.location,
                       "expected keyword 'let'; this is an internal error");
    }

    const auto name_token = lexer.next();
    if (name_token.kind != TokenKind::Identifier) {
        return ERR_PTR(err::SyntaxError, lexer, name_token.location,
                       "expected variable name after 'let'");
    }
    const auto let_name = name_token.location.substr();

    const auto eq_token = lexer.next();
    if (eq_token.kind != TokenKind::Equal) {
        return ERR_PTR(err::SyntaxError, lexer, eq_token.location,
                       "expected '=' between variable name and initial value");
    }

    auto value = parse_whole_expression(lexer);
    FORWARD_ERROR(value);

    // TODO: Handle the 'mutable' keyword
    bool mutable_ = true;
    return ast::LetExpression::alloc(std::move(let_name), std::move(value).value(), mutable_);
}

util::Result<std::unique_ptr<ast::ReturnExpression>> parse_return(Lexer& lexer) {
    const auto token = lexer.next();
    if (token.kind != TokenKind::Return) {
        return ERR_PTR(err::SyntaxError, lexer, token.location,
                       "expected keyword 'return'; this is an internal error");
    }

    auto expr = parse_whole_expression(lexer);
    FORWARD_ERROR(expr);

    return ast::ReturnExpression::alloc(std::move(expr).value());
}

util::Result<ast::ExpressionPtr> parse_whole_expression(Lexer& lexer) {
    // Handle the expressions that are only allowed at the top level
    const auto token = lexer.peek();
    switch (token.kind) {
        case TokenKind::Let:
            return parse_let(lexer);

        case TokenKind::Return:
            return parse_return(lexer);

        default:
            return parse_standard_expression(lexer);
    }
}

util::Result<std::unique_ptr<ast::ExportExpression>> parse_export(Lexer& lexer) {
    const auto export_token = lexer.next();
    if (export_token.kind != TokenKind::Export) {
        return ERR_PTR(err::SyntaxError, lexer, export_token.location,
                       "expected keyword 'export'; this is an internal error");
    }

    const auto next_token = lexer.peek();
    switch (next_token.kind) {
        case TokenKind::Fn: {
            auto result = parse_function(lexer);
            FORWARD_ERROR(result);
            return ast::ExportExpression::alloc(std::move(result).value(), export_token.location);
        }

        default:
            return ERR_PTR(err::SyntaxError, lexer, next_token.location,
                           "unexpected token while parsing export");
    }
}

}  // namespace rain::lang
