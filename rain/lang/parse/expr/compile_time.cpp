#include "rain/lang/ast/expr/compile_time.hpp"

#include <memory>

#include "rain/lang/ast/scope/scope.hpp"
#include "rain/lang/err/syntax.hpp"
#include "rain/lang/lex/lexer.hpp"
#include "rain/util/result.hpp"

namespace rain::lang::parse {

util::Result<std::unique_ptr<ast::Expression>> parse_any_expression(lex::Lexer& lexer,
                                                                    ast::Scope& scope);

util::Result<std::unique_ptr<ast::CompileTimeExpression>> parse_compile_time(lex::Lexer& lexer,
                                                                             ast::Scope& scope) {
    const auto hash_token = lexer.next();
    IF_DEBUG {
        if (hash_token.kind != lex::TokenKind::Hash) {
            return ERR_PTR(err::SyntaxError, hash_token.location,
                           "expected '#'; this is an internal error");
        }
    }

    auto expression = parse_any_expression(lexer, scope);
    FORWARD_ERROR(expression);

    return std::make_unique<ast::CompileTimeExpression>(std::move(expression).value(),
                                                        hash_token.location);
}

}  // namespace rain::lang::parse
