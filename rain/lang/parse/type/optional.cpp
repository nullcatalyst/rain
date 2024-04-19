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

    auto type_result = parse_any_type(lexer, scope);
    FORWARD_ERROR(type_result);
    auto type = std::move(type_result).value();

    const auto location = question_token.location.merge(type->location());
    if (type->kind() == serial::TypeKind::Optional) {
        return ERR_PTR(err::SyntaxError, location,
                       "optional types cannot be nested inside another optional type");
    }

    return &type->get_optional_type(scope);
}

}  // namespace rain::lang::parse
