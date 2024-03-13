#include "rain/lang/parser.hpp"

#include <array>

#include "absl/strings/numbers.h"
#include "rain/ast/expr/all.hpp"
#include "rain/ast/type/all.hpp"
#include "rain/err/all.hpp"
#include "rain/lang/parse+common.hpp"
#include "rain/util/colors.hpp"
#include "rain/util/result.hpp"

namespace rain::lang {

util::Result<ast::TypePtr> Parser::parse_type(Lexer& lexer) { return parse_whole_type(lexer); }

util::Result<ast::ExpressionPtr> Parser::parse_expression(Lexer& lexer) {
    return parse_whole_expression(lexer);
}

util::Result<ParsedModule> Parser::parse(Lexer& lexer) {
    std::vector<ast::ExpressionPtr> expressions;

    for (;;) {
        const auto token = lexer.peek();
        switch (token.kind) {
            case TokenKind::Struct: {
                auto result = parse_type(lexer);
                FORWARD_ERROR(result);
                expressions.emplace_back(
                    ast::TypeDeclarationExpression::alloc(std::move(result).value()));
                break;
            }

            case TokenKind::Export: {
                auto result = parse_export(lexer);
                FORWARD_ERROR(result);
                expressions.emplace_back(std::move(result).value());
                break;
            }

            case TokenKind::Fn: {
                auto result = parse_function(lexer);
                FORWARD_ERROR(result);
                expressions.emplace_back(std::move(result).value());
                break;
            }

            case TokenKind::Eof: {
                return ParsedModule{std::move(expressions)};
            }

            default: {
                return ERR_PTR(err::SyntaxError, lexer, token.location,
                               "unexpected top-level token");
            }
        }
    }

    // __builtin_unreachable();
}

}  // namespace rain::lang
