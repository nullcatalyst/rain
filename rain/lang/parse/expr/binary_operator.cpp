#include "rain/lang/ast/expr/binary_operator.hpp"

#include <array>
#include <memory>

#include "rain/lang/ast/expr/cast.hpp"
#include "rain/lang/ast/scope/scope.hpp"
#include "rain/lang/lex/lexer.hpp"
#include "rain/lang/serial/kind.hpp"
#include "rain/util/result.hpp"
#include "rain/util/unreachable.hpp"

namespace rain::lang::parse {

util::Result<std::unique_ptr<ast::Expression>> parse_atom(lex::Lexer& lexer, ast::Scope& scope);

util::Result<absl::Nonnull<ast::Type*>> parse_any_type(lex::Lexer& lexer, ast::Scope& scope);

namespace {

enum Precedence : int {
    Unknown        = 0,
    Assignment     = 10,  // Assign
    Logical        = 20,  // And, Or, Xor
    Comparative    = 30,  // Eq, Ne, Lt, Gt, Le, Ge
    Bitwise        = 40,  // Shl, Shr, Rotl, Rotr
    Additive       = 50,  // Add, Sub
    Multiplicative = 60,  // Mul, Div, Rem
    Cast           = 70,  // As
    Subscript      = 80,  // ArrayIndex
};

static constexpr const std::array<int, 22> BINARY_OPERATOR_PRECEDENCE{
    Precedence::Unknown,  // Unknown,
    // Assignment
    Precedence::Assignment,  // Assign,
    // Arithmetic
    Precedence::Additive,        // Add,
    Precedence::Additive,        // Subtract,
    Precedence::Multiplicative,  // Multiply,
    Precedence::Multiplicative,  // Divide,
    Precedence::Multiplicative,  // Remainder,
    // Comparison
    Precedence::Comparative,  // Equal,
    Precedence::Comparative,  // NotEqual,
    Precedence::Comparative,  // Less,
    Precedence::Comparative,  // Greater,
    Precedence::Comparative,  // LessEqual,
    Precedence::Comparative,  // GreaterEqual,
    // Bitwise
    Precedence::Logical,  // And,
    Precedence::Logical,  // Or,
    Precedence::Bitwise,  // Xor,
    Precedence::Bitwise,  // ShiftLeft,
    Precedence::Bitwise,  // ShiftRight,
    Precedence::Bitwise,  // RotateLeft,
    Precedence::Bitwise,  // RotateRight,
    // Conversion
    Precedence::Subscript,  // ArrayIndex,
    Precedence::Cast,       // As,
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
            return serial::BinaryOperatorKind::Remainder;
        case lex::TokenKind::Ampersand:
            return serial::BinaryOperatorKind::And;
        case lex::TokenKind::Pipe:
            return serial::BinaryOperatorKind::Or;
        case lex::TokenKind::Caret:
            return serial::BinaryOperatorKind::Xor;
        case lex::TokenKind::LessLess:
            return serial::BinaryOperatorKind::ShiftLeft;
        case lex::TokenKind::GreaterGreater:
            return serial::BinaryOperatorKind::ShiftRight;
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
        case lex::TokenKind::As:
            return serial::BinaryOperatorKind::CastFrom;
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

        if (binop == serial::BinaryOperatorKind::CastFrom) {
            auto cast_type_result = parse_any_type(lexer, scope);
            FORWARD_ERROR(cast_type_result);

            auto cast_type     = std::move(cast_type_result).value();
            auto type_location = cast_type->location();
            lhs_expression     = std::make_unique<ast::CastExpression>(
                std::move(lhs_expression), std::move(cast_type), binop_token.location,
                type_location);
            continue;
        }

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
    return parse_rhs(lexer, scope, std::move(lhs_expression), Precedence::Unknown);
}

}  // namespace rain::lang::parse
