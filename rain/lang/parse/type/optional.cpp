#include <memory>

#include "absl/base/nullability.h"
#include "rain/lang/ast/scope/scope.hpp"
#include "rain/lang/ast/type/type.hpp"
#include "rain/lang/err/syntax.hpp"
#include "rain/lang/lex/lexer.hpp"
#include "rain/util/result.hpp"

namespace rain::lang::parse {

util::Result<absl::Nonnull<ast::Type*>> parse_any_type(lex::Lexer& lexer, ast::Scope& scope);

util::Result<absl::Nonnull<ast::OptionalType*>> parse_optional_type(lex::Lexer& lexer,
                                                                    ast::Scope& scope) {
    const auto question_token = lexer.next();
    IF_DEBUG {
        if (question_token.kind != lex::TokenKind::Question) {
            return ERR_PTR(err::SyntaxError, question_token.location,
                           "expected '?'; this is an internal error");
        }
    }

    auto wrapped_type_result = parse_any_type(lexer, scope);
    FORWARD_ERROR(wrapped_type_result);
    auto wrapped_type = std::move(wrapped_type_result).value();

    const auto location = question_token.location.merge(wrapped_type->location());
    if (wrapped_type->kind() == serial::TypeKind::Optional) {
        return ERR_PTR(err::SyntaxError, location, "nested optional types are not allowed");
    }

    return &wrapped_type->get_optional_type();
}

}  // namespace rain::lang::parse
