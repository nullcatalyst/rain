#include <utility>

#include "absl/base/nullability.h"
#include "absl/strings/str_cat.h"
#include "rain/lang/ast/scope/scope.hpp"
#include "rain/lang/ast/type/interface.hpp"
#include "rain/lang/ast/type/struct.hpp"
#include "rain/lang/ast/type/type.hpp"
#include "rain/lang/ast/type/unresolved.hpp"
#include "rain/lang/err/syntax.hpp"
#include "rain/lang/lex/lexer.hpp"
#include "rain/util/unreachable.hpp"

namespace rain::lang::parse {

util::Result<absl::Nonnull<ast::ArrayType*>> parse_array_type(lex::Lexer& lexer, ast::Scope& scope);
util::Result<absl::Nonnull<ast::OptionalType*>>   parse_optional_type(lex::Lexer& lexer,
                                                                      ast::Scope& scope);
util::Result<absl::Nonnull<ast::StructType*>>     parse_struct_type(lex::Lexer& lexer,
                                                                    ast::Scope& scope);
util::Result<absl::Nonnull<ast::InterfaceType*>>  parse_interface_type(lex::Lexer& lexer,
                                                                       ast::Scope& scope);
util::Result<absl::Nonnull<ast::UnresolvedType*>> parse_unresolved_type(lex::Lexer& lexer,
                                                                        ast::Scope& scope);

util::Result<absl::Nonnull<ast::Type*>> parse_any_type(lex::Lexer& lexer, ast::Scope& scope) {
    const auto token = lexer.peek();
    switch (token.kind) {
        case lex::TokenKind::Struct:
            return parse_struct_type(lexer, scope);

        case lex::TokenKind::Interface:
            return parse_interface_type(lexer, scope);

        case lex::TokenKind::LSquareBracket:
            return parse_array_type(lexer, scope);

        case lex::TokenKind::Question:
            return parse_optional_type(lexer, scope);

        case lex::TokenKind::Identifier:
            return parse_unresolved_type(lexer, scope);

        default:
            return ERR_PTR(err::SyntaxError, token.location,
                           absl::StrCat("unexpected token: ", token.text()));
    }

    util::unreachable();
}

}  // namespace rain::lang::parse
