#include "rain/lang/ast/expr/extern.hpp"

#include <memory>

#include "rain/lang/ast/expr/function_declaration.hpp"
#include "rain/lang/ast/var/block.hpp"
#include "rain/lang/err/syntax.hpp"
#include "rain/lang/lex/lexer.hpp"
#include "rain/lang/lex/string_value.hpp"
#include "rain/lang/parse/util/list.hpp"
#include "rain/util/result.hpp"

namespace rain::lang::parse {

util::Result<std::unique_ptr<ast::FunctionDeclarationExpression>> parse_function_declaration(
    lex::Lexer& lexer, ast::Scope& scope);

util::Result<std::unique_ptr<ast::ExternExpression>> parse_extern(lex::Lexer& lexer,
                                                                  ast::Scope& scope) {
    const auto extern_token = lexer.next();
    IF_DEBUG {
        if (extern_token.kind != lex::TokenKind::Extern) {
            return ERR_PTR(err::SyntaxError, extern_token.location,
                           "expected keyword 'extern'; this is an internal error");
        }
    }

    if (const auto lparen_token = lexer.next();
        lparen_token.kind != lex::TokenKind::LRoundBracket) {
        return ERR_PTR(err::SyntaxError, lparen_token.location, "expected '(' before extern keys");
    }

    llvm::SmallVector<std::string, 3> keys;
    auto                              keys_result = parse_list(
        lexer, lex::TokenKind::Comma, lex::TokenKind::RRoundBracket,
        [&](lex::Lexer& lexer) -> util::Result<std::string> {
            const auto key_token = lexer.next();
            if (key_token.kind != lex::TokenKind::String) {
                return ERR_PTR(err::SyntaxError, key_token.location,
                                                            "expected string literal for extern key");
            }

            keys.emplace_back(lex::string_value(key_token.text()));
            return {};
        },
        [](lex::Lexer& lexer, lex::Token token) -> util::Result<std::string> {
            return ERR_PTR(err::SyntaxError, token.location,
                                                        "expected ',' or ')' after extern key");
        });
    FORWARD_ERROR(keys_result);

    lexer.next();  // Consume the ')' token

    if (const auto fn_token = lexer.peek(); fn_token.kind != lex::TokenKind::Fn) {
        return ERR_PTR(err::SyntaxError, fn_token.location,
                       "expected function declaration after extern keys");
    }

    auto function_declaration_result = parse_function_declaration(lexer, scope);
    FORWARD_ERROR(function_declaration_result);
    auto function_declaration = std::move(function_declaration_result).value();

    const auto location = extern_token.location.merge(function_declaration->location());
    return std::make_unique<ast::ExternExpression>(std::move(keys), std::move(function_declaration),
                                                   location);
}

}  // namespace rain::lang::parse
