#include "rain/lang/ast/expr/module.hpp"

#include "rain/lang/ast/expr/export.hpp"
#include "rain/lang/err/syntax.hpp"
#include "rain/lang/parse/all.hpp"
#include "rain/lang/parse/list.hpp"

namespace rain::lang::parse {

namespace {

util::Result<std::unique_ptr<ast::Expression>> parse_top_level_expression(lex::Lexer& lexer,
                                                                          ast::Scope& scope) {
    auto token = lexer.peek();
    switch (token.kind) {
        case lex::TokenKind::Fn: {
            auto result = parse_function(lexer, scope);
            FORWARD_ERROR(result);
            return std::move(result).value();
        }

        case lex::TokenKind::Struct: {
            auto result = parse_struct(lexer, scope);
            FORWARD_ERROR(result);
            auto  type     = std::move(result).value();
            auto* type_ptr = type.get();
            if (type->is_named()) {
                scope.add_type(type->name_or_empty(), std::move(type));
            }
            return std::make_unique<ast::TypeExpression>(type_ptr);
        }

        case lex::TokenKind::Export: {
            lexer.next();  // Consume the `export` token

            auto result = parse_top_level_expression(lexer, scope);
            FORWARD_ERROR(result);
            return std::make_unique<ast::ExportExpression>(std::move(result).value(),
                                                           token.location);
        }

        default:
            return ERR_PTR(err::SyntaxError, token.location,
                           absl::StrCat("unexpected token \"", token.text(), "\""));
    }

    return {};
}

}  // namespace

util::Result<std::unique_ptr<ast::Module>> parse_module(lex::Lexer&        lexer,
                                                        ast::BuiltinScope& builtin) {
    auto  module = std::make_unique<ast::Module>(builtin);
    auto& scope  = module->scope();

    auto result =
        parse_many(lexer, lex::TokenKind::EndOfFile, [&](lex::Lexer& lexer) -> util::Result<void> {
            auto result = parse_top_level_expression(lexer, scope);
            FORWARD_ERROR(result);
            module->add_expression(std::move(result).value());
            return {};
        });
    FORWARD_ERROR(result);

    return module;
}

}  // namespace rain::lang::parse
