#include "rain/lang/ast/expr/function.hpp"

#include <memory>

#include "rain/lang/err/syntax.hpp"
#include "rain/lang/lex/lexer.hpp"
#include "rain/util/result.hpp"

namespace rain::lang::parse {

util::Result<std::unique_ptr<ast::FunctionExpression>> parse_function(lex::Lexer& lexer,
                                                                      Scope&      scope) {
    const auto function_token = lexer.next();
    if (function_token.kind != TokenKind::Fn) {
        // This function should only be called if we already know the next token starts a function.
        return ERR_PTR(err::SyntaxError, lexer, function_token.location,
                       "expected keyword 'fn'; this is an internal error");
    }

    absl::Nullable<ast::Type*> callee_type = nullptr;
    {
        const auto state = lexer.save_state();

        auto type_result = parse_any_type(lexer);
        if (const auto period_token = lexer.peek(); period_token.kind == TokenKind::Period) {
            lexer.next();  // Consume the '.' token

            // Ensure that the owning type parsed successfully.
            FORWARD_ERROR(type_result);

            callee_type = std::move(type_result).value();
        } else {
            // This is not a method, so restore the lexer state, so that any parsed tokens are
            // treated as a function name instead of a type name.
            lexer.restore_state(state);
        }
    }

    std::optional<std::string_view> fn_name;
    auto                            next_token = lexer.next();
    if (next_token.kind == TokenKind::Identifier) {
        fn_name    = next_token.text();
        next_token = lexer.next();
    } else if (callee_type != nullptr) {
        return ERR_PTR(
            err::SyntaxError, lexer, next_token.location,
            "a function name is required when defining a function that can be called as a "
            "method");
    }

    if (next_token.kind != TokenKind::LRoundBracket) {
        return ERR_PTR(err::SyntaxError, lexer, next_token.location,
                       "expected '(' before function arguments");
    }

    std::vector<ast::FunctionArgument> arguments;
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

        const auto argument_name = next_token.text();

        next_token = lexer.next();
        if (next_token.kind != TokenKind::Colon) {
            return ERR_PTR(err::SyntaxError, lexer, next_token.location,
                           "expected ':' after function argument name");
        }

        auto argument_type = parse_any_type(lexer);
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

}  // namespace rain::lang::parse
