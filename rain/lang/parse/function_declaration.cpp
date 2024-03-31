#include "rain/lang/ast/expr/function_declaration.hpp"

#include <memory>

#include "rain/lang/ast/expr/method.hpp"
#include "rain/lang/ast/var/block.hpp"
#include "rain/lang/err/syntax.hpp"
#include "rain/lang/lex/lexer.hpp"
#include "rain/lang/parse/all.hpp"
#include "rain/lang/parse/list.hpp"
#include "rain/util/result.hpp"

namespace rain::lang::parse {

util::Result<std::unique_ptr<ast::FunctionDeclarationExpression>> parse_function_declaration(
    lex::Lexer& lexer, ast::Scope& scope) {
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
                       "function declaration is not allowed in this context");
    }

    const auto name_token = lexer.next();
    if (name_token.kind != lex::TokenKind::Identifier) {
        return ERR_PTR(err::SyntaxError, name_token.location,
                       "a name is required when declaring a function");
    }
    const std::string_view fn_name = name_token.text();

    if (const auto lparen_token = lexer.next();
        lparen_token.kind != lex::TokenKind::LRoundBracket) {
        return ERR_PTR(err::SyntaxError, lparen_token.location,
                       "expected '(' before function arguments");
    }

    ast::ArgumentList arguments;
    auto              result = parse_list(
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

            arguments.emplace_back(std::make_unique<ast::BlockVariable>(
                argument_name.text(), std::move(argument_type).value()));
            return {};
        },
        [](lex::Lexer& lexer, lex::Token token) -> util::Result<void> {
            return ERR_PTR(err::SyntaxError, token.location,
                                        "expected ',' or ')' after function argument");
        });
    FORWARD_ERROR(result);

    const auto rparen_location = lexer.next().location;  // Consume the ')'

    util::MaybeOwnedPtr<ast::Type> return_type = nullptr;
    if (const auto arrow_token = lexer.peek(); arrow_token.kind == lex::TokenKind::RArrow) {
        lexer.next();  // Consume the '->' token

        auto return_type_result = parse_any_type(lexer, scope);
        FORWARD_ERROR(return_type_result);

        return_type = std::move(return_type_result).value();
    }

    const auto declaration_location = return_type != nullptr
                                          ? function_token.location.merge(return_type->location())
                                          : function_token.location.merge(rparen_location);

    return std::make_unique<ast::FunctionDeclarationExpression>(
        std::move(fn_name), std::move(arguments), std::move(return_type), declaration_location);
}

}  // namespace rain::lang::parse
