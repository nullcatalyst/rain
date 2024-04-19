#include <memory>

#include "rain/lang/ast/type/type.hpp"
#include "rain/lang/err/syntax.hpp"
#include "rain/lang/lex/lexer.hpp"
#include "rain/lang/parse/util/int_value.hpp"
#include "rain/lang/parse/util/list.hpp"
#include "rain/util/result.hpp"

namespace rain::lang::parse {

util::Result<absl::Nonnull<ast::Type*>> parse_any_type(lex::Lexer& lexer, ast::Scope& scope);

util::Result<absl::Nonnull<ast::ArrayType*>> parse_array_type(lex::Lexer& lexer,
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

    // auto location = lsquare_token.location.merge(element_type->location());
    return &element_type->get_array_type(scope, std::move(length).value());
}

}  // namespace rain::lang::parse
