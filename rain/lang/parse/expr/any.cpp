#include <utility>

#include "absl/strings/str_cat.h"
#include "rain/lang/ast/expr/expression.hpp"
#include "rain/lang/ast/scope/scope.hpp"
#include "rain/lang/err/syntax.hpp"
#include "rain/lang/lex/lexer.hpp"
#include "rain/util/unreachable.hpp"

namespace rain::lang::parse {

util::Result<std::unique_ptr<ast::Expression>> parse_atom(lex::Lexer& lexer, ast::Scope& scope);
util::Result<std::unique_ptr<ast::Expression>> parse_rhs(
    lex::Lexer& lexer, ast::Scope& scope, std::unique_ptr<ast::Expression> lhs_expression);

util::Result<std::unique_ptr<ast::Expression>> parse_any_expression(lex::Lexer& lexer,
                                                                    ast::Scope& scope) {
    auto lhs = parse_atom(lexer, scope);
    FORWARD_ERROR(lhs);

    return parse_rhs(lexer, scope, std::move(lhs).value());
}

}  // namespace rain::lang::parse
