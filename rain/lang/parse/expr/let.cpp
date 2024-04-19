#include "rain/lang/ast/expr/let.hpp"

#include <memory>

#include "rain/lang/ast/expr/compile_time.hpp"
#include "rain/lang/ast/scope/scope.hpp"
#include "rain/lang/err/syntax.hpp"
#include "rain/lang/lex/lexer.hpp"
#include "rain/util/result.hpp"

namespace rain::lang::parse {

util::Result<std::unique_ptr<ast::Expression>> parse_any_expression(lex::Lexer& lexer,
                                                                    ast::Scope& scope);

util::Result<std::unique_ptr<ast::LetExpression>> parse_let(lex::Lexer& lexer, ast::Scope& scope,
                                                            bool global) {
    const auto let_token = lexer.next();
    IF_DEBUG {
        if (let_token.kind != lex::TokenKind::Let) {
            return ERR_PTR(err::SyntaxError, let_token.location,
                           "expected 'let'; this is an internal error");
        }
    }

    const auto name_token = lexer.next();
    if (name_token.kind != lex::TokenKind::Identifier) {
        return ERR_PTR(err::SyntaxError, name_token.location,
                       "expected identifier for variable name");
    }

    if (const auto eq_token = lexer.next(); eq_token.kind != lex::TokenKind::Equal) {
        return ERR_PTR(err::SyntaxError, eq_token.location,
                       "expected '=' between variable name and initial value");
    }

    auto value_result = parse_any_expression(lexer, scope);
    FORWARD_ERROR(value_result);

    auto value = std::move(value_result).value();
    if (global && value->kind() != serial::ExpressionKind::CompileTime) {
        // All global variables must be compile-time constants. That way no initialization function
        // needs to be called to set their initial value.
        const lex::Location location = value->location().empty_string_before();
        value = std::make_unique<ast::CompileTimeExpression>(std::move(value), location);
    }

    return std::make_unique<ast::LetExpression>(name_token.text(), std::move(value), global,
                                                let_token.location, name_token.location);
}

}  // namespace rain::lang::parse
