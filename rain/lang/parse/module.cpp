#include "rain/lang/ast/module.hpp"

#include "rain/lang/ast/expr/export.hpp"
#include "rain/lang/ast/expr/function.hpp"
#include "rain/lang/ast/expr/interface_implementation.hpp"
#include "rain/lang/ast/expr/let.hpp"
#include "rain/lang/ast/expr/type.hpp"
#include "rain/lang/ast/type/interface.hpp"
#include "rain/lang/ast/type/struct.hpp"
#include "rain/lang/err/syntax.hpp"
#include "rain/lang/parse/util/list.hpp"

namespace rain::lang::parse {

// Expressions
util::Result<std::unique_ptr<ast::FunctionExpression>> parse_function(
    lex::Lexer& lexer, ast::Scope& scope, bool allow_callee_type, bool allow_self_argument,
    absl::Nullable<ast::Type*> default_callee_type);
util::Result<std::unique_ptr<ast::InterfaceImplementationExpression>>
parse_interface_implementation(lex::Lexer& lexer, ast::Scope& scope);
util::Result<std::unique_ptr<ast::LetExpression>> parse_let(lex::Lexer& lexer, ast::Scope& scope,
                                                            bool global);

// Types
util::Result<absl::Nonnull<ast::StructType*>>    parse_struct_type(lex::Lexer& lexer,
                                                                   ast::Scope& scope);
util::Result<absl::Nonnull<ast::InterfaceType*>> parse_interface_type(lex::Lexer& lexer,
                                                                      ast::Scope& scope);

namespace {

util::Result<std::unique_ptr<ast::Expression>> parse_top_level_expression(lex::Lexer& lexer,
                                                                          ast::Scope& scope) {
    auto token = lexer.peek();
    switch (token.kind) {
        case lex::TokenKind::Fn: {
            return parse_function(lexer, scope, true, true, nullptr);
        }

        case lex::TokenKind::Struct: {
            auto result = parse_struct_type(lexer, scope);
            FORWARD_ERROR(result);
            return std::make_unique<ast::TypeExpression>(std::move(result).value());
        }

        case lex::TokenKind::Interface: {
            auto result = parse_interface_type(lexer, scope);
            FORWARD_ERROR(result);
            return std::make_unique<ast::TypeExpression>(std::move(result).value());
        }

        case lex::TokenKind::Impl: {
            return parse_interface_implementation(lexer, scope);
        }

        case lex::TokenKind::Let: {
            return parse_let(lexer, scope, true);
        }

        case lex::TokenKind::Export: {
            lexer.next();  // Consume the `export` token

            auto result = parse_top_level_expression(lexer, scope);
            FORWARD_ERROR(result);
            return std::make_unique<ast::ExportExpression>(std::move(result).value(),
                                                           token.location);
        }

        default:
            return ERR_PTR(
                err::SyntaxError, token.location,
                absl::StrCat("while trying to parse top-level expression, found unexpected token: ",
                             token.text()));
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
