#include "rain/lang/ast/expr/function.hpp"

#include <memory>

#include "rain/lang/ast/expr/method.hpp"
#include "rain/lang/ast/var/block.hpp"
#include "rain/lang/err/syntax.hpp"
#include "rain/lang/lex/lexer.hpp"
#include "rain/lang/parse/all.hpp"
#include "rain/lang/parse/list.hpp"
#include "rain/util/result.hpp"

namespace rain::lang::parse {

util::Result<std::unique_ptr<ast::FunctionExpression>> parse_function(lex::Lexer& lexer,
                                                                      ast::Scope& scope) {
    const auto function_token = lexer.next();
    IF_DEBUG {
        if (function_token.kind != lex::TokenKind::Fn) {
            return ERR_PTR(err::SyntaxError, function_token.location,
                           "expected keyword 'fn'; this is an internal error");
        }
    }

    const absl::Nullable<ast::ModuleScope*> module_scope = scope.module();
    if (module_scope == nullptr) {
        return ERR_PTR(err::SyntaxError, function_token.location,
                       "function definition is not allowed in this context");
    }

    util::MaybeOwnedPtr<ast::Type> callee_type = nullptr;
    {
        const auto state = lexer.save_state();

        auto type_result = parse_any_type(lexer, scope);
        if (const auto period_token = lexer.peek(); period_token.kind == lex::TokenKind::Period) {
            FORWARD_ERROR(type_result);
            callee_type = std::move(type_result).value();

            lexer.next();  // Consume the '.' token
        } else {
            // This is not a method, so restore the lexer state, so that any parsed tokens are
            // treated as a function name instead of a type name.
            lexer.restore_state(state);
        }
    }

    std::string_view fn_name;
    lex::Location    fn_name_location;
    auto             next_token = lexer.next();
    if (next_token.kind == lex::TokenKind::Identifier) {
        fn_name          = next_token.text();
        fn_name_location = next_token.location;
        next_token       = lexer.next();
    } else if (callee_type != nullptr) {
        return ERR_PTR(err::SyntaxError, next_token.location,
                       "a function name is required when defining a function with a callee type");
    }

    if (next_token.kind != lex::TokenKind::LRoundBracket) {
        return ERR_PTR(err::SyntaxError, next_token.location,
                       "expected '(' before function arguments");
    }

    auto  body       = std::make_unique<ast::BlockExpression>(*module_scope);
    auto& body_scope = body->scope();

    // Check if the method takes a 'self' argument.
    bool              has_self_argument = false;
    ast::ArgumentList arguments;
    if (callee_type != nullptr) {
        if (auto self_token = lexer.peek(); self_token.kind == lex::TokenKind::Self) {
            lexer.next();  // Consume the 'self' token

            auto end_or_sep_token = lexer.peek();
            if (end_or_sep_token.kind != lex::TokenKind::Comma &&
                end_or_sep_token.kind != lex::TokenKind::RRoundBracket) {
                return ERR_PTR(err::SyntaxError, end_or_sep_token.location,
                               "expected ',' or ')' after function argument");
            }

            if (end_or_sep_token.kind == lex::TokenKind::Comma) {
                lexer.next();  // Consume the ',' token
            }

            has_self_argument = true;

            auto self_argument = std::make_unique<ast::BlockVariable>("self", callee_type.get());
            arguments.push_back(self_argument.get());
            body_scope.add_variable("self", std::move(self_argument));
        }
    }

    auto result = parse_list(
        lexer, lex::TokenKind::Comma, lex::TokenKind::RRoundBracket,
        [&](lex::Lexer& lexer) -> util::Result<void> {
            const auto argument_name = lexer.next();
            if (argument_name.kind != lex::TokenKind::Identifier) {
                return ERR_PTR(err::SyntaxError, argument_name.location,
                               "expected identifier for function argument name");
            }

            if (const auto colon_token = lexer.next(); colon_token.kind != lex::TokenKind::Colon) {
                return ERR_PTR(err::SyntaxError, colon_token.location,
                               "expected ':' between argument name and argument type");
            }

            auto argument_type = parse_any_type(lexer, scope);
            FORWARD_ERROR(argument_type);

            auto argument = std::make_unique<ast::BlockVariable>(argument_name.text(),
                                                                 std::move(argument_type).value());
            arguments.push_back(argument.get());
            body_scope.add_variable(argument_name.text(), std::move(argument));
            return {};
        },
        [](lex::Lexer& lexer, lex::Token token) -> util::Result<void> {
            return ERR_PTR(err::SyntaxError, token.location,
                           "expected ',' or ')' after function argument");
        });
    FORWARD_ERROR(result);

    const auto rparen_location = lexer.next().location;  // Consume the ')'

    util::MaybeOwnedPtr<ast::Type> return_type = nullptr;
    next_token                                 = lexer.peek();
    if (next_token.kind == lex::TokenKind::RArrow) {
        lexer.next();  // Consume the '->' token

        auto return_type_result = parse_any_type(lexer, scope);
        FORWARD_ERROR(return_type_result);

        return_type = std::move(return_type_result).value();
    }

    const auto lbracket_token = lexer.next();
    if (lbracket_token.kind != lex::TokenKind::LCurlyBracket) {
        // This function should only be called if we already know the next token starts a block.
        return ERR_PTR(err::SyntaxError, lbracket_token.location,
                       "expected '{' before function body");
    }

    auto body_result = parse_many(
        lexer, lex::TokenKind::RCurlyBracket, [&](lex::Lexer& lexer) -> util::Result<void> {
            auto expression_result = parse_any_expression(lexer, body_scope);
            FORWARD_ERROR(expression_result);
            auto expression = std::move(expression_result).value();
            body->add_expression(std::move(expression));
            return {};
        });
    FORWARD_ERROR(body_result);

    lex::Location rbracket_location = lexer.next().location;  // Consume the '}'
    body->set_location(lbracket_token.location.merge(rbracket_location));

    const auto declaration_location = return_type != nullptr
                                          ? function_token.location.merge(return_type->location())
                                          : function_token.location.merge(rparen_location);

    if (callee_type != nullptr) {
        return std::make_unique<ast::MethodExpression>(
            std::move(callee_type), fn_name, std::move(arguments), std::move(return_type),
            std::move(body), has_self_argument, declaration_location, callee_type->location());
    }
    return std::make_unique<ast::FunctionExpression>(std::move(fn_name), std::move(arguments),
                                                     std::move(return_type), std::move(body),
                                                     declaration_location);
}

}  // namespace rain::lang::parse
