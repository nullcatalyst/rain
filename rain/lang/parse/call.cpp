#include <memory>

#include "absl/strings/str_cat.h"
#include "rain/lang/ast/expr/expression.hpp"
#include "rain/lang/ast/scope/scope.hpp"
#include "rain/lang/err/syntax.hpp"
#include "rain/lang/lex/lexer.hpp"
#include "rain/lang/parse/all.hpp"
#include "rain/lang/parse/list.hpp"
#include "rain/util/result.hpp"

namespace rain::lang::parse {

util::Result<std::unique_ptr<ast::CallExpression>> parse_call(
    lex::Lexer& lexer, ast::Scope& scope, std::unique_ptr<ast::Expression> callee) {
    const auto lbracket_token = lexer.next();
    if (lbracket_token.kind != lex::TokenKind::LRoundBracket) {
        return ERR_PTR(err::SyntaxError, lexer, lbracket_token.location,
                       "expected '('; this is an internal error");
    }

    llvm::SmallVector<std::unique_ptr<ast::Expression>, 4> arguments;
    auto                                                   result = parse_list(
        lexer, lex::TokenKind::Comma, lex::TokenKind::RRoundBracket,
        [&](lex::Lexer& lexer) -> util::Result<void> {
            auto argument = parse_any_expression(lexer, scope);
            FORWARD_ERROR(argument);

            arguments.emplace_back(std::move(argument).value());
            return {};
        },
        [&](lex::Lexer& lexer, lex::Token token) -> util::Result<void> {
            return ERR_PTR(err::SyntaxError, lexer, token.location, "expected ',' or ')'");
        });
    FORWARD_ERROR(result);

    return std::make_unique<ast::CallExpression>(std::move(callee), std::move(arguments));
}

}  // namespace rain::lang::parse
