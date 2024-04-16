#include "rain/lang/ast/expr/function.hpp"

#include <memory>

#include "absl/base/nullability.h"
#include "rain/lang/ast/type/type.hpp"
#include "rain/lang/ast/var/block.hpp"
#include "rain/lang/err/syntax.hpp"
#include "rain/lang/lex/lexer.hpp"
#include "rain/lang/parse/util/list.hpp"
#include "rain/util/result.hpp"

namespace rain::lang::parse {

util::Result<std::unique_ptr<ast::Expression>> parse_any_expression(lex::Lexer& lexer,
                                                                    ast::Scope& scope);

util::Result<absl::Nonnull<ast::Type*>> parse_any_type(lex::Lexer& lexer, ast::Scope& scope);

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
    IF_DEBUG {
        if (module_scope == nullptr) {
            return ERR_PTR(err::SyntaxError, function_token.location,
                           "function definition is not allowed in this context");
        }
    }

    absl::Nullable<ast::Type*> callee_type = nullptr;
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

    auto name_token = lexer.next();
    if (name_token.kind != lex::TokenKind::Identifier) {
        return ERR_PTR(err::SyntaxError, name_token.location, "expected function name");
    }

    if (auto lparen_token = lexer.next(); lparen_token.kind != lex::TokenKind::LRoundBracket) {
        return ERR_PTR(err::SyntaxError, lparen_token.location,
                       "expected '(' before function arguments");
    }

    auto  body       = std::make_unique<ast::BlockExpression>(*module_scope);
    auto& body_scope = body->scope();

    // Check if the method takes a 'self' argument.
    bool              has_self_argument = false;
    ast::ArgumentList arguments;
    if (callee_type != nullptr) {
        auto* self_type = callee_type;
        if (auto ampersand_token = lexer.peek();
            ampersand_token.kind == lex::TokenKind::Ampersand) {
            lexer.next();  // Consume the '&' token

            self_type = &callee_type->get_reference_type();
        }

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

            auto self_argument =
                std::make_unique<ast::BlockVariable>("self", self_type, /*mutable*/ false);
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

            auto argument = std::make_unique<ast::BlockVariable>(
                argument_name.text(), std::move(argument_type).value(), /*mutable*/ false);
            arguments.emplace_back(argument.get());
            body_scope.add_variable(argument_name.text(), std::move(argument));
            return {};
        },
        [](lex::Lexer& lexer, lex::Token token) -> util::Result<void> {
            return ERR_PTR(err::SyntaxError, token.location,
                           "expected ',' or ')' after function argument");
        });
    FORWARD_ERROR(result);

    const auto rparen_location = lexer.next().location;  // Consume the ')'

    absl::Nullable<ast::Type*> return_type = nullptr;
    if (auto arrow_token = lexer.peek(); arrow_token.kind == lex::TokenKind::RArrow) {
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

    ast::Scope::TypeList argument_types;
    argument_types.reserve(arguments.size());
    for (const auto& argument : arguments) {
        argument_types.emplace_back(argument->type());
    }
    auto* function_type =
        scope.find_or_create_unresolved_function_type(callee_type, argument_types, return_type);
    auto* function_variable =
        scope.create_unresolved_function(name_token.text(), function_type, name_token.location);

    return std::make_unique<ast::FunctionExpression>(
        function_variable, std::move(arguments), function_type, std::move(body),
        declaration_location, return_type != nullptr ? return_type->location() : lex::Location());
}

}  // namespace rain::lang::parse
