#include "rain/lang/ast/expr/binary_operator.hpp"

#include <array>
#include <memory>

#include "rain/lang/ast/scope/scope.hpp"
#include "rain/lang/lex/lexer.hpp"
#include "rain/lang/serial/kind.hpp"
#include "rain/util/result.hpp"
#include "rain/util/unreachable.hpp"

namespace rain::lang::parse {

util::Result<std::unique_ptr<ast::Expression>> parse_atom(lex::Lexer& lexer, ast::Scope& scope);

namespace {

enum Precendence : int {
    Unknown        = 0,
    Assignment     = 10,  // Assign
    Logical        = 20,  // And, Or
    Comparative    = 30,  // Eq, Ne, Lt, Gt, Le, Ge
    Bitwise        = 40,  // Xor, Shl, Shr
    Additive       = 50,  // Add, Sub
    Multiplicative = 60,  // Mul, Div, Mod
};

constexpr std::array<int, 18> BINARY_OPERATOR_PRECEDENCE{
    Precendence::Unknown,         // Unknown,
    Precendence::Assignment,      // Assign,
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

serial::BinaryOperatorKind get_operator_for_token(lex::Token token) {
    switch (token.kind) {
        case lex::TokenKind::Equal:
            return serial::BinaryOperatorKind::Assign;
        case lex::TokenKind::Plus:
            return serial::BinaryOperatorKind::Add;
        case lex::TokenKind::Minus:
            return serial::BinaryOperatorKind::Subtract;
        case lex::TokenKind::Star:
            return serial::BinaryOperatorKind::Multiply;
        case lex::TokenKind::Slash:
            return serial::BinaryOperatorKind::Divide;
        case lex::TokenKind::Percent:
            return serial::BinaryOperatorKind::Modulo;
        case lex::TokenKind::Ampersand:
            return serial::BinaryOperatorKind::And;
        case lex::TokenKind::Pipe:
            return serial::BinaryOperatorKind::Or;
        case lex::TokenKind::Caret:
            return serial::BinaryOperatorKind::Xor;
        case lex::TokenKind::LessLess:
            return serial::BinaryOperatorKind::LeftShift;
        case lex::TokenKind::GreaterGreater:
            return serial::BinaryOperatorKind::RightShift;
        case lex::TokenKind::EqualEqual:
            return serial::BinaryOperatorKind::Equal;
        case lex::TokenKind::ExclaimEqual:
            return serial::BinaryOperatorKind::NotEqual;
        case lex::TokenKind::Less:
            return serial::BinaryOperatorKind::Less;
        case lex::TokenKind::Greater:
            return serial::BinaryOperatorKind::Greater;
        case lex::TokenKind::LessEqual:
            return serial::BinaryOperatorKind::LessEqual;
        case lex::TokenKind::GreaterEqual:
            return serial::BinaryOperatorKind::GreaterEqual;
        default:
            return serial::BinaryOperatorKind::Unknown;
    }
}

util::Result<std::unique_ptr<ast::Expression>> parse_rhs(
    lex::Lexer& lexer, ast::Scope& scope, std::unique_ptr<ast::Expression> lhs_expression,
    int lhs_precedence) {
    for (;;) {
        const auto binop_token = lexer.peek();
        const auto binop       = get_operator_for_token(binop_token);
        if (binop == serial::BinaryOperatorKind::Unknown) {
            return lhs_expression;
        }

        int binop_precedence = BINARY_OPERATOR_PRECEDENCE[static_cast<int>(binop)];
        if (binop_precedence < lhs_precedence) {
            return lhs_expression;
        }
        lexer.next();  // Consume the operator

        auto rhs_expression = parse_atom(lexer, scope);
        FORWARD_ERROR(rhs_expression);

        const auto next_token = lexer.peek();
        const auto next_binop = get_operator_for_token(next_token);
        if (next_binop != serial::BinaryOperatorKind::Unknown) {
            const auto rhs_precedence = BINARY_OPERATOR_PRECEDENCE[static_cast<int>(next_binop)];
            if (binop_precedence < rhs_precedence) {
                rhs_expression =
                    parse_rhs(lexer, scope, std::move(rhs_expression).value(), binop_precedence);
                FORWARD_ERROR(rhs_expression);
            }
        }

        lhs_expression = std::make_unique<ast::BinaryOperatorExpression>(
            std::move(lhs_expression), std::move(rhs_expression).value(), binop,
            binop_token.location);
    }

    util::unreachable();
}

}  // namespace

util::Result<std::unique_ptr<ast::Expression>> parse_rhs(
    lex::Lexer& lexer, ast::Scope& scope, std::unique_ptr<ast::Expression> lhs_expression) {
    return parse_rhs(lexer, scope, std::move(lhs_expression), Precendence::Unknown);
}

}  // namespace rain::lang::parse
