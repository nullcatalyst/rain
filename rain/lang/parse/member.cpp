#include <memory>

#include "absl/strings/str_cat.h"
#include "rain/lang/ast/expr/expression.hpp"
#include "rain/lang/ast/scope/scope.hpp"
#include "rain/lang/err/syntax.hpp"
#include "rain/lang/lex/lexer.hpp"
#include "rain/lang/parse/all.hpp"
#include "rain/util/result.hpp"

namespace rain::lang::parse {

util::Result<std::unique_ptr<ast::MemberExpression>> parse_member(
    lex::Lexer& lexer, ast::Scope& scope, std::unique_ptr<ast::Expression> owner) {
    const auto period_token = lexer.next();
    if (period_token.kind != lex::TokenKind::Period) {
        return ERR_PTR(err::SyntaxError, period_token.location,
                       "expected '.'; this is an internal error");
    }

    const auto member_token = lexer.next();
    if (member_token.kind != lex::TokenKind::Identifier) {
        return ERR_PTR(err::SyntaxError, member_token.location, "expected identifier after '.'");
    }

    return std::make_unique<ast::MemberExpression>(std::move(owner), member_token.text(),
                                                   member_token.location);
}

}  // namespace rain::lang::parse
