#include "rain/lang/ast/expr/interface_implementation.hpp"

#include <memory>

#include "absl/base/nullability.h"
#include "rain/lang/ast/type/type.hpp"
#include "rain/lang/ast/var/block.hpp"
#include "rain/lang/err/syntax.hpp"
#include "rain/lang/lex/lexer.hpp"
#include "rain/lang/parse/util/list.hpp"
#include "rain/util/result.hpp"

namespace rain::lang::parse {

util::Result<std::unique_ptr<ast::FunctionExpression>> parse_function(
    lex::Lexer& lexer, ast::Scope& scope, bool allow_callee_type, bool allow_self_argument,
    absl::Nullable<ast::Type*> default_callee_type);

util::Result<absl::Nonnull<ast::Type*>> parse_any_type(lex::Lexer& lexer, ast::Scope& scope);

util::Result<std::unique_ptr<ast::InterfaceImplementationExpression>>
parse_interface_implementation(lex::Lexer& lexer, ast::Scope& scope) {
    const auto impl_token = lexer.next();
    IF_DEBUG {
        if (impl_token.kind != lex::TokenKind::Impl) {
            return ERR_PTR(err::SyntaxError, impl_token.location,
                           "expected keyword 'impl'; this is an internal error");
        }
    }

    const absl::Nullable<ast::ModuleScope*> module_scope = scope.module();
    IF_DEBUG {
        if (module_scope == nullptr) {
            return ERR_PTR(err::SyntaxError, impl_token.location,
                           "impl definition is not allowed in this context");
        }
    }

    auto implementer_type_result = parse_any_type(lexer, scope);
    FORWARD_ERROR(implementer_type_result);

    if (const auto colon_token = lexer.next(); colon_token.kind != lex::TokenKind::Colon) {
        return ERR_PTR(err::SyntaxError, colon_token.location, "expected ':' after impl type");
    }

    auto interface_type_result = parse_any_type(lexer, scope);
    FORWARD_ERROR(interface_type_result);

    if (const auto lbracket_token = lexer.next();
        lbracket_token.kind != lex::TokenKind::LCurlyBracket) {
        return ERR_PTR(err::SyntaxError, lbracket_token.location, "expected '{' before impl body");
    }

    std::vector<std::unique_ptr<ast::FunctionExpression>> functions;
    auto                                                  result = parse_many(
        lexer, lex::TokenKind::RCurlyBracket, [&](lex::Lexer& lexer) -> util::Result<void> {
            auto function_result =
                parse_function(lexer, scope, false, true, std::move(interface_type_result).value());
            FORWARD_ERROR(function_result);
            functions.emplace_back(std::move(function_result).value());
            return {};
        });

    const auto rbracket_token = lexer.next();

    return std::make_unique<ast::InterfaceImplementationExpression>(
        std::move(implementer_type_result).value(), std::move(interface_type_result).value(),
        std::move(functions), impl_token.location.merge(rbracket_token.location));
}

}  // namespace rain::lang::parse
