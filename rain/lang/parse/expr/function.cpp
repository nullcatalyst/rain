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

util::Result<std::unique_ptr<ast::FunctionDeclarationExpression>> parse_function_declaration(
    lex::Lexer& lexer, ast::Scope& scope, bool allow_callee_type, bool allow_self_argument,
    absl::Nullable<ast::Type*> default_callee_type);

util::Result<std::unique_ptr<ast::FunctionExpression>> parse_function(
    lex::Lexer& lexer, ast::Scope& scope, bool allow_callee_type, bool allow_self_argument,
    absl::Nullable<ast::Type*> default_callee_type) {
    auto function_declaration_result = parse_function_declaration(
        lexer, scope, allow_callee_type, allow_self_argument, default_callee_type);
    FORWARD_ERROR(function_declaration_result);
    auto function_declaration = std::move(function_declaration_result).value();

    const auto lbracket_token = lexer.next();
    if (lbracket_token.kind != lex::TokenKind::LCurlyBracket) {
        // This function should only be called if we already know the next token starts a block.
        return ERR_PTR(err::SyntaxError, lbracket_token.location,
                       "expected '{' before function body");
    }

    auto  body       = std::make_unique<ast::BlockExpression>(function_declaration->scope());
    auto& body_scope = body->scope();

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

    return std::make_unique<ast::FunctionExpression>(std::move(*function_declaration),
                                                     std::move(body));
}

}  // namespace rain::lang::parse
