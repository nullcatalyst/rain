#include "rain/lang/ast/type/interface.hpp"

#include <memory>

#include "absl/base/nullability.h"
#include "rain/lang/err/syntax.hpp"
#include "rain/lang/lex/lexer.hpp"
#include "rain/lang/parse/util/list.hpp"
#include "rain/util/result.hpp"

namespace rain::lang::parse {

util::Result<std::unique_ptr<ast::FunctionDeclarationExpression>> parse_function_declaration(
    lex::Lexer& lexer, ast::Scope& scope, bool allow_callee_type, bool allow_self_argument,
    absl::Nullable<ast::Type*> default_callee_type);

util::Result<absl::Nonnull<ast::InterfaceType*>> parse_interface_type(lex::Lexer& lexer,
                                                                      ast::Scope& scope) {
    const auto interface_token = lexer.next();
    IF_DEBUG {
        if (interface_token.kind != lex::TokenKind::Interface) {
            return ERR_PTR(err::SyntaxError, interface_token.location,
                           "expected keyword 'interface'; this is an internal error");
        }
    }

    auto name_token = lexer.next();
    if (name_token.kind != lex::TokenKind::Identifier) {
        return ERR_PTR(err::SyntaxError, name_token.location,
                       "expected identifier for interface name");
    }
    std::string_view interface_name = name_token.text();

    if (auto lbracket_token = lexer.next(); lbracket_token.kind != lex::TokenKind::LCurlyBracket) {
        return ERR_PTR(err::SyntaxError, lbracket_token.location,
                       "expected '{' after interface name");
    }

    auto interface_type = std::make_unique<ast::InterfaceType>(interface_name);

    auto result = parse_many(
        lexer, lex::TokenKind::RCurlyBracket, [&](lex::Lexer& lexer) -> util::Result<void> {
            auto result =
                parse_function_declaration(lexer, scope, false, true, interface_type.get());
            FORWARD_ERROR(result);
            interface_type->add_method(std::move(result).value());
            return {};
        });
    FORWARD_ERROR(result);

    const auto rbracket_token = lexer.next();  // Consume the '}'
    interface_type->set_location(interface_token.location.merge(rbracket_token.location));

    return static_cast<ast::InterfaceType*>(
        scope.add_named_type(interface_name, std::move(interface_type)));
}

}  // namespace rain::lang::parse
