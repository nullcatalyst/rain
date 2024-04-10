#include <memory>

#include "rain/lang/ast/type/type.hpp"
#include "rain/lang/err/syntax.hpp"
#include "rain/lang/lex/lexer.hpp"
#include "rain/lang/parse/util/int_value.hpp"
#include "rain/lang/parse/util/list.hpp"
#include "rain/util/result.hpp"

namespace rain::lang::parse {

util::Result<absl::Nonnull<ast::Type*>> parse_any_type(lex::Lexer& lexer, ast::Scope& scope);

util::Result<absl::Nonnull<ast::ReferenceType*>> parse_reference_type(lex::Lexer& lexer,
                                                                      ast::Scope& scope) {
    const auto ampersand_token = lexer.next();
    IF_DEBUG {
        if (ampersand_token.kind != lex::TokenKind::Ampersand) {
            return ERR_PTR(err::SyntaxError, ampersand_token.location,
                           "expected '&'; this is an internal error");
        }
    }

    auto type_result = parse_any_type(lexer, scope);
    FORWARD_ERROR(type_result);
    auto type = std::move(type_result).value();

    const auto location = ampersand_token.location.merge(type->location());
    if (type->kind() == serial::TypeKind::Reference) {
        return ERR_PTR(err::SyntaxError, location,
                       "reference types cannot be nested inside another reference type");
    }

    return &type->get_reference_type();
}

}  // namespace rain::lang::parse
