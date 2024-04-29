#include <memory>

#include "rain/lang/ast/type/type.hpp"
#include "rain/lang/err/syntax.hpp"
#include "rain/lang/lex/lexer.hpp"
#include "rain/lang/parse/util/int_value.hpp"
#include "rain/lang/parse/util/list.hpp"
#include "rain/util/result.hpp"

namespace rain::lang::parse {

util::Result<absl::Nonnull<ast::Type*>> parse_any_type(lex::Lexer& lexer, ast::Scope& scope);

util::Result<absl::Nonnull<ast::Type*>> parse_array_or_slice_type(lex::Lexer& lexer,
                                                                  ast::Scope& scope) {
    const auto lsquare_token = lexer.next();
    IF_DEBUG {
        if (lsquare_token.kind != lex::TokenKind::LSquareBracket) {
            return ERR_PTR(err::SyntaxError, lsquare_token.location,
                           "expected '['; this is an internal error");
        }
    }

    std::optional<uint32_t> array_length;
    auto                    length_or_rsquare_token = lexer.next();
    if (length_or_rsquare_token.kind == lex::TokenKind::Integer) {
        auto array_length_result = int_value<uint32_t>(length_or_rsquare_token);
        FORWARD_ERROR(array_length_result);
        array_length = std::move(array_length_result).value();

        length_or_rsquare_token = lexer.next();
    }

    if (length_or_rsquare_token.kind != lex::TokenKind::RSquareBracket) {
        if (array_length.has_value()) {
            return ERR_PTR(err::SyntaxError, length_or_rsquare_token.location,
                           "expected ']' after array length");
        } else {
            return ERR_PTR(err::SyntaxError, length_or_rsquare_token.location,
                           "expected ']' for a slice type, or integer literal for array length of "
                           "an array type");
        }
    }

    auto element_type_result = parse_any_type(lexer, scope);
    FORWARD_ERROR(element_type_result);
    auto element_type = std::move(element_type_result).value();

    // auto location = lsquare_token.location.merge(element_type->location());
    if (array_length.has_value()) {
        return &element_type->get_array_type(scope, array_length.value());
    }
    return &element_type->get_slice_type(scope);
}

}  // namespace rain::lang::parse
