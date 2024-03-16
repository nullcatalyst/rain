#include "rain/lang/ast/expr/module.hpp"

#include "rain/lang/err/syntax.hpp"
#include "rain/lang/parse/all.hpp"
#include "rain/lang/parse/list.hpp"

namespace rain::lang::parse {

util::Result<std::unique_ptr<ast::Module>> parse_module(lex::Lexer&        lexer,
                                                        ast::BuiltinScope& builtin) {
    auto  module = std::make_unique<ast::Module>(builtin);
    auto& scope  = module->scope();

    auto result =
        parse_many(lexer, lex::TokenKind::EndOfFile, [&](lex::Lexer& lexer) -> util::Result<void> {
            auto token = lexer.peek();

#define HANDLE_EXPR(expr_result)                                \
    do {                                                        \
        FORWARD_ERROR(expr_result);                             \
        module->add_expression(std::move(expr_result).value()); \
    } while (false)

#define HANDLE_TYPE(type_result)                                                 \
    do {                                                                         \
        FORWARD_ERROR(type_result);                                              \
        auto  type     = std::move(type_result).value();                         \
        auto* type_ptr = type.get();                                             \
        scope.add_type(std::move(type));                                         \
        module->add_expression(std::make_unique<ast::TypeExpression>(type_ptr)); \
    } while (false)

            switch (token.kind) {
                case lex::TokenKind::Fn: {
                    auto result = parse_function(lexer, scope);
                    FORWARD_ERROR(result);
                    module->add_expression(std::move(result).value());
                    break;
                }

                case lex::TokenKind::Struct: {
                    auto result = parse_struct(lexer, scope);
                    FORWARD_ERROR(result);
                    auto  type     = std::move(result).value();
                    auto* type_ptr = type.get();
                    if (type->is_named()) {
                        scope.add_type(type->name_or_empty(), std::move(type));
                    }
                    module->add_expression(std::make_unique<ast::TypeExpression>(type_ptr));
                    break;
                }

                default:
                    return ERR_PTR(err::SyntaxError, lexer, token.location,
                                   absl::StrCat("unexpected token \"", token.text(), "\""));
            }

            return {};

#undef HANDLE_EXPR
#undef HANDLE_TYPE
        });
    FORWARD_ERROR(result);

    return module;
}

}  // namespace rain::lang::parse
