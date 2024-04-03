#include <memory>

#include "rain/lang/err/syntax.hpp"
#include "rain/lang/lex/lexer.hpp"
#include "rain/lang/parse/all.hpp"
#include "rain/lang/parse/list.hpp"
#include "rain/lang/parse/util/int_value.hpp"
#include "rain/util/result.hpp"

namespace rain::lang::parse {

util::Result<std::unique_ptr<ast::OptionalType>> parse_optional_type(lex::Lexer& lexer,
                                                                     ast::Scope& scope) {
    const auto question_token = lexer.next();
    IF_DEBUG {
        if (question_token.kind != lex::TokenKind::Question) {
            return ERR_PTR(err::SyntaxError, lsquare_token.location,
                           "expected '?'; this is an internal error");
        }
    }

    auto wrapped_type_result = parse_any_type(lexer, scope);
    FORWARD_ERROR(wrapped_type);
    auto wrapped_type = std::move(wrapped_type_result).value();

    const auto location = question_token.location.merge(wrapped_type->location());
    if (wrapped_type->kind() == serial::TypeKind::Optional) {
        return ERR(err::SyntaxError, location, "nested optional types are not allowed");
    }

    return std::make_unique<ast::OptionalType>(std::move(element_type), location);
}

}  // namespace rain::lang::parse
