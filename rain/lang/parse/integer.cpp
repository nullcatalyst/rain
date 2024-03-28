#include "rain/lang/ast/expr/integer.hpp"

#include <memory>

#include "rain/lang/ast/scope/scope.hpp"
#include "rain/lang/err/syntax.hpp"
#include "rain/lang/lex/lexer.hpp"
#include "rain/util/result.hpp"

namespace rain::lang::parse {

util::Result<std::unique_ptr<ast::IntegerExpression>> parse_integer(lex::Lexer& lexer,
                                                                    ast::Scope& scope) {
    const auto integer_token = lexer.next();
    if (integer_token.kind != lex::TokenKind::Integer) {
        // This function should only be called if we already know the next token is an integer.
        return ERR_PTR(err::SyntaxError, integer_token.location,
                       "expected integer literal; this is an internal error");
    }

    constexpr uint64_t MAX   = std::numeric_limits<uint64_t>::max() / 10;
    uint64_t           value = 0;

    for (const char c : integer_token.location.text()) {
        if (value > MAX) {
            // Multiplying by 10 will overflow
            return ERR_PTR(err::SyntaxError, integer_token.location,
                           "integer literal is too large");
        }
        value *= 10;

        if (value > std::numeric_limits<uint64_t>::max() - (c - '0')) {
            // Adding the next digit will overflow
            return ERR_PTR(err::SyntaxError, integer_token.location,
                           "integer literal is too large");
        }
        value += c - '0';
    }

    return std::make_unique<ast::IntegerExpression>(value, integer_token.location);
}

}  // namespace rain::lang::parse
