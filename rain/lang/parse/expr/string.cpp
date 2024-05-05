#include "rain/lang/ast/expr/string.hpp"

#include <memory>

#include "rain/lang/ast/scope/scope.hpp"
#include "rain/lang/err/syntax.hpp"
#include "rain/lang/lex/lexer.hpp"
#include "rain/lang/parse/util/str_value.hpp"
#include "rain/util/result.hpp"

namespace rain::lang::parse {

util::Result<std::unique_ptr<ast::StringExpression>> parse_string(lex::Lexer& lexer,
                                                                  ast::Scope& scope) {
    const auto string_token = lexer.next();
    IF_DEBUG {
        if (string_token.kind != lex::TokenKind::String) {
            return ERR_PTR(err::SyntaxError, string_token.location,
                           "expected string literal; this is an internal error");
        }
    }

    auto value = str_value(string_token);
    FORWARD_ERROR(value);

    return std::make_unique<ast::StringExpression>(std::move(value).value(), string_token.location);
}

}  // namespace rain::lang::parse
