#include "rain/lang/ast/expr/unary_operator.hpp"

#include <memory>

#include "rain/lang/ast/scope/scope.hpp"
#include "rain/lang/err/syntax.hpp"
#include "rain/lang/lex/lexer.hpp"
#include "rain/lang/serial/kind.hpp"
#include "rain/util/result.hpp"

namespace rain::lang::parse {

util::Result<std::unique_ptr<ast::Expression>> parse_atom(lex::Lexer& lexer, ast::Scope& scope);

util::Result<std::unique_ptr<ast::Expression>> parse_unary_operator(lex::Lexer& lexer,
                                                                    ast::Scope& scope) {
    serial::UnaryOperatorKind op = serial::UnaryOperatorKind::Unknown;

    const auto op_token = lexer.peek();
    switch (op_token.kind) {
        case lex::TokenKind::Minus:
            op = serial::UnaryOperatorKind::Negative;
            break;
        case lex::TokenKind::Plus:
            op = serial::UnaryOperatorKind::Positive;
            break;
        case lex::TokenKind::Exclaim:
            op = serial::UnaryOperatorKind::Not;
            break;
        default:
            break;
    }

    if (op == serial::UnaryOperatorKind::Unknown) {
        return ERR_PTR(
            err::SyntaxError, op_token.location,
            absl::StrCat("while trying to parse unary operator, found unexpected token: ",
                         op_token.text()));
    }

    lexer.next();  // Consume the operator

    auto rhs = parse_atom(lexer, scope);
    FORWARD_ERROR(rhs);
    return std::make_unique<ast::UnaryOperatorExpression>(std::move(rhs).value(), op,
                                                          op_token.location);
}

}  // namespace rain::lang::parse
