#include "rain/lang/ast/expr/float.hpp"

#include <memory>

#include "absl/strings/numbers.h"
#include "rain/lang/ast/scope/scope.hpp"
#include "rain/lang/err/syntax.hpp"
#include "rain/lang/lex/lexer.hpp"
#include "rain/util/result.hpp"

namespace rain::lang::parse {

util::Result<std::unique_ptr<ast::FloatExpression>> parse_float(lex::Lexer& lexer,
                                                                ast::Scope& scope) {
    const auto float_token = lexer.next();
    if (float_token.kind != lex::TokenKind::Float) {
        // This function should only be called if we already know the next token is an integer.
        return ERR_PTR(err::SyntaxError, lexer, float_token.location,
                       "expected float literal; this is an internal error");
    }

    double value = 0.0;

    const std::string_view text = float_token.location.text();
    const char*            it   = text.data();
    const char*            end  = it + text.size();

    // Parse the integer part of the float
    for (const char c : text) {
        ++it;

        if (c == '.') {
            break;
        }

        value *= 10.0;
        value += static_cast<double>(c - '0');
    }

    // Parse the fractional part of the float
    double multiplier = 0.1;
    for (const char c : std::string_view{it + 1, end}) {
        value += static_cast<double>(c - '0') * multiplier;
        multiplier *= 0.1;
    }

    return std::make_unique<ast::FloatExpression>(value);
}

}  // namespace rain::lang::parse
