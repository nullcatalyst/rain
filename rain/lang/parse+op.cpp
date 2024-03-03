#include "rain/ast/expr/all.hpp"
#include "rain/err/all.hpp"
#include "rain/lang/lexer.hpp"
#include "rain/lang/parse+common.hpp"
#include "rain/util/result.hpp"

namespace rain::lang {

enum Precendence : int {
    Unknown        = 0,
    Logical        = 10,  // And, Or
    Comparative    = 20,  // Eq, Ne, Lt, Gt, Le, Ge
    Bitwise        = 30,  // Xor, Shl, Shr
    Additive       = 40,  // Add, Sub
    Multiplicative = 50,  // Mul, Div, Mod
};
constexpr std::array<int, 17> BINARY_OPERATOR_PRECEDENCE{
    Precendence::Unknown,         // Unknown,
    Precendence::Additive,        // Add,
    Precendence::Additive,        // Sub,
    Precendence::Multiplicative,  // Mul,
    Precendence::Multiplicative,  // Div,
    Precendence::Multiplicative,  // Mod,
    Precendence::Logical,         // And,
    Precendence::Logical,         // Or,
    Precendence::Bitwise,         // Xor,
    Precendence::Bitwise,         // Shl,
    Precendence::Bitwise,         // Shr,
    Precendence::Comparative,     // Eq,
    Precendence::Comparative,     // Ne,
    Precendence::Comparative,     // Lt,
    Precendence::Comparative,     // Gt,
    Precendence::Comparative,     // Le,
    Precendence::Comparative,     // Ge,
};

ast::BinaryOperatorKind get_operator_for_token(Token token) {
    switch (token.kind) {
        case TokenKind::Plus:
            return ast::BinaryOperatorKind::Add;
        case TokenKind::Minus:
            return ast::BinaryOperatorKind::Subtract;
        case TokenKind::Star:
            return ast::BinaryOperatorKind::Multiply;
        case TokenKind::Slash:
            return ast::BinaryOperatorKind::Divide;
        case TokenKind::Percent:
            return ast::BinaryOperatorKind::Modulo;
        case TokenKind::Ampersand:
            return ast::BinaryOperatorKind::And;
        case TokenKind::Pipe:
            return ast::BinaryOperatorKind::Or;
        case TokenKind::Caret:
            return ast::BinaryOperatorKind::Xor;
        case TokenKind::LessLess:
            return ast::BinaryOperatorKind::ShiftLeft;
        case TokenKind::GreaterGreater:
            return ast::BinaryOperatorKind::ShiftRight;
        case TokenKind::EqualEqual:
            return ast::BinaryOperatorKind::Equal;
        case TokenKind::ExclaimEqual:
            return ast::BinaryOperatorKind::NotEqual;
        case TokenKind::Less:
            return ast::BinaryOperatorKind::Less;
        case TokenKind::Greater:
            return ast::BinaryOperatorKind::Greater;
        case TokenKind::LessEqual:
            return ast::BinaryOperatorKind::LessEqual;
        case TokenKind::GreaterEqual:
            return ast::BinaryOperatorKind::GreaterEqual;
        default:
            return ast::BinaryOperatorKind::Unknown;
    }
}

util::Result<ast::ExpressionPtr> parse_rhs(Lexer& lexer, ast::ExpressionPtr lhs_expression,
                                           int lhs_precedence) {
    for (;;) {
        const auto binop_token = lexer.peek();
        const auto binop       = get_operator_for_token(binop_token);
        if (binop == ast::BinaryOperatorKind::Unknown) {
            return std::move(lhs_expression);
        }

        int binop_precedence = BINARY_OPERATOR_PRECEDENCE[static_cast<int>(binop)];
        if (binop_precedence < lhs_precedence) {
            return std::move(lhs_expression);
        }
        lexer.next();  // Consume the operator

        auto rhs_expression = parse_atom(lexer);
        FORWARD_ERROR(rhs_expression);

        const auto next_token = lexer.peek();
        const auto next_binop = get_operator_for_token(next_token);
        if (next_binop != ast::BinaryOperatorKind::Unknown) {
            const auto rhs_precedence = BINARY_OPERATOR_PRECEDENCE[static_cast<int>(next_binop)];
            if (binop_precedence < rhs_precedence) {
                rhs_expression =
                    parse_rhs(lexer, std::move(rhs_expression).value(), binop_precedence);
                FORWARD_ERROR(rhs_expression);
            }
        }

        lhs_expression = ast::BinaryOperatorExpression::alloc(
            std::move(lhs_expression), std::move(rhs_expression).value(), binop);
    }
}

// TODO: Find a better name for this function.
util::Result<ast::ExpressionPtr> parse_standard_expression(Lexer& lexer) {
    // Default to parsing any type of expression that can be combined with operators
    auto lhs = parse_atom(lexer);
    FORWARD_ERROR(lhs);

    return parse_rhs(lexer, std::move(lhs).value(), Precendence::Unknown);
}

}  // namespace rain::lang
