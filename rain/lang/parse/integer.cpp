#include "rain/lang/ast/expr/integer.hpp"

#include <memory>

#include "rain/lang/ast/scope/scope.hpp"
#include "rain/lang/err/syntax.hpp"
#include "rain/lang/lex/lexer.hpp"
#include "rain/lang/parse/util/int_value.hpp"
#include "rain/util/assert.hpp"
#include "rain/util/result.hpp"

namespace rain::lang::parse {

util::Result<std::unique_ptr<ast::IntegerExpression>> parse_integer(lex::Lexer& lexer,
                                                                    ast::Scope& scope) {
    const auto integer_token = lexer.next();
    IF_DEBUG {
        if (integer_token.kind != lex::TokenKind::Integer) {
            return ERR_PTR(err::SyntaxError, integer_token.location,
                           "expected integer literal; this is an internal error");
        }
    }

    auto value = int_value<int64_t>(integer_token);
    FORWARD_ERROR(value);

    return std::make_unique<ast::IntegerExpression>(std::move(value).value(),
                                                    integer_token.location);
}

}  // namespace rain::lang::parse
