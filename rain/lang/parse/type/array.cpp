#include <memory>

#include "rain/lang/err/syntax.hpp"
#include "rain/lang/lex/lexer.hpp"
#include "rain/lang/parse/all.hpp"
#include "rain/lang/parse/list.hpp"
#include "rain/lang/parse/util/int_value.hpp"
#include "rain/util/result.hpp"

namespace rain::lang::parse {

util::Result<std::unique_ptr<ast::ArrayType>> parse_array_type(lex::Lexer& lexer,
                                                               ast::Scope& scope) {
    const auto lsquare_token = lexer.next();
    IF_DEBUG {
        if (lsquare_token.kind != lex::TokenKind::LSquareBracket) {
            return ERR_PTR(err::SyntaxError, lsquare_token.location,
                           "expected '['; this is an internal error");
        }
    }

    auto length_token = lexer.next();
    if (length_token.kind != lex::TokenKind::Integer) {
        return ERR_PTR(err::SyntaxError, length_token.location,
                       "expected integer literal for array length");
    }
    auto length = int_value<size_t>(length_token);
    FORWARD_ERROR(length);

    if (auto rsquare_token = lexer.next(); rsquare_token.kind != lex::TokenKind::RSquareBracket) {
        return ERR_PTR(err::SyntaxError, rsquare_token.location, "expected ']' after array length");
    }

    auto element_type_result = parse_any_type(lexer, scope);
    FORWARD_ERROR(element_type_result);
    auto element_type = std::move(element_type_result).value();

    auto location = lsquare_token.location.merge(element_type->location());
    return std::make_unique<ast::ArrayType>(std::move(element_type), std::move(length).value(),
                                            location);
}

}  // namespace rain::lang::parse
