#include "rain/lang/ast/expr/module.hpp"

#include "rain/lang/ast/expr/export.hpp"
#include "rain/lang/err/syntax.hpp"
#include "rain/lang/parse/list.hpp"

namespace rain::lang::parse {

util::Result<std::unique_ptr<ast::FunctionExpression>> parse_function(lex::Lexer& lexer,
                                                                      ast::Scope& scope);

namespace {

util::Result<std::unique_ptr<ast::Expression>> parse_top_level_expression(lex::Lexer& lexer,
                                                                          ast::Scope& scope) {
    auto token = lexer.peek();
    switch (token.kind) {
        case lex::TokenKind::Fn: {
            auto result = parse_function(lexer, scope);
            FORWARD_ERROR(result);
            auto function = std::move(result).value();
            function->add_to_scope(scope);
            return std::move(function);
        }

        case lex::TokenKind::Struct: {
            auto result = parse_struct_type(lexer, scope);
            FORWARD_ERROR(result);
            return std::make_unique<ast::TypeExpression>(type_ptr);
        }

        case lex::TokenKind::Interface: {
            auto result = parse_interface_type(lexer, scope);
            FORWARD_ERROR(result);
            auto  type     = std::move(result).value();
            auto* type_ptr = type.get();
            scope.add_type(type_ptr->name(), std::move(type));
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
                           absl::StrCat("unexpected token: ", token.text()));
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
