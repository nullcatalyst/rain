#include "cirrus/lang/parser.hpp"

#include <array>

#include "absl/strings/numbers.h"
#include "cirrus/ast/expr/all.hpp"
#include "cirrus/ast/type/all.hpp"
#include "cirrus/err/all.hpp"
#include "cirrus/util/colors.hpp"
#include "cirrus/util/result.hpp"

namespace cirrus::lang {

namespace {

util::Result<ast::TypePtr> parse_whole_type(Lexer& lexer);

util::Result<std::shared_ptr<ast::StructType>> parse_struct(Lexer& lexer) {
    const auto struct_token = lexer.next();
    if (struct_token.kind != TokenKind::Struct) {
        return ERR_PTR(err::SyntaxError, lexer, struct_token.location, "expected keyword 'struct'");
    }

    std::optional<util::String> struct_name;
    auto                        next_token = lexer.next();

    // The struct name is optional
    if (next_token.kind == TokenKind::Identifier) {
        struct_name = next_token.location.substr();
        next_token  = lexer.next();
    }

    if (next_token.kind != TokenKind::LCurlyBracket) {
        return ERR_PTR(err::SyntaxError, lexer, next_token.location,
                       "expected '{' after struct identifier");
    }

    std::vector<ast::StructTypeFieldData> struct_fields;
    for (;;) {
        // Handle the struct fields
        auto next_token = lexer.next();

        switch (next_token.kind) {
            case TokenKind::RCurlyBracket:
                return ast::StructType::alloc(std::move(struct_name), std::move(struct_fields));

            case TokenKind::Identifier:
                // Found a field name
                break;

            default:
                return ERR_PTR(err::SyntaxError, lexer, next_token.location,
                               "expected identifier for struct field name");
        }

        const auto field_name = next_token.location.substr();

        next_token = lexer.next();
        if (next_token.kind != TokenKind::Colon) {
            return ERR_PTR(err::SyntaxError, lexer, next_token.location,
                           "expected ':' after struct field name");
        }

        auto field_type = parse_whole_type(lexer);
        FORWARD_ERROR(field_type);

        struct_fields.emplace_back(ast::StructTypeFieldData{
            .name = field_name,
            .type = std::move(field_type).value(),
        });

        next_token = lexer.next();
        if (next_token.kind != TokenKind::Semicolon) {
            return ERR_PTR(err::SyntaxError, lexer, next_token.location,
                           "expected ';' after struct field definition");
        }
    }

    __builtin_unreachable();
}

util::Result<ast::TypePtr> parse_whole_type(Lexer& lexer) {
    const auto token = lexer.peek();
    switch (token.kind) {
        case TokenKind::Struct:
            return parse_struct(lexer);

        case TokenKind::Identifier:
            lexer.next();  // Consume the identifier token
            return ast::UnresolvedType::alloc(token.location.substr());

        default:
            return ERR_PTR(err::SyntaxError, lexer, token.location, "unexpected token");
    }
}

util::Result<ast::ExpressionPtr> parse_whole_expression(Lexer& lexer);

util::Result<std::unique_ptr<ast::IntegerExpression>> parse_integer(Lexer& lexer) {
    const auto token = lexer.next();
    if (token.kind != TokenKind::Integer) {
        return ERR_PTR(err::SyntaxError, lexer, token.location, "expected integer literal");
    }

    constexpr uint64_t MAX   = std::numeric_limits<uint64_t>::max() / 10;
    uint64_t           value = 0;

    for (const char c : token.location.substr()) {
        if (value > MAX) {
            // Multiplying by 10 will overflow
            return ERR_PTR(err::SyntaxError, lexer, token.location, "integer literal is too large");
        }
        value *= 10;

        if (value > std::numeric_limits<uint64_t>::max() - (c - '0')) {
            // Adding the next digit will overflow
            return ERR_PTR(err::SyntaxError, lexer, token.location, "integer literal is too large");
        }
        value += c - '0';
    }

    return ast::IntegerExpression::alloc(value);
}

util::Result<std::unique_ptr<ast::FloatExpression>> parse_float(Lexer& lexer) {
    const auto token = lexer.next();
    if (token.kind != TokenKind::Float) {
        return ERR_PTR(err::SyntaxError, lexer, token.location, "expected floating point literal");
    }

    double value = 0.0;
    if (!absl::SimpleAtod(token.location.substr(), &value)) {
        return ERR_PTR(err::SyntaxError, lexer, token.location, "invalid floating point literal");
    }

    return ast::FloatExpression::alloc(value);
}

util::Result<std::unique_ptr<ast::IdentifierExpression>> parse_identifier(Lexer& lexer) {
    const auto token = lexer.next();
    if (token.kind != TokenKind::Identifier) {
        return ERR_PTR(err::SyntaxError, lexer, token.location, "expected identifier");
    }

    return ast::IdentifierExpression::alloc(token.location.substr());
}

util::Result<std::unique_ptr<ast::ParenthesisExpression>> parse_parenthesis(Lexer& lexer) {
    const auto lround_token = lexer.next();
    if (lround_token.kind != TokenKind::LRoundBracket) {
        return ERR_PTR(err::SyntaxError, lexer, lround_token.location, "expected '('");
    }

    auto expr = parse_whole_expression(lexer);
    FORWARD_ERROR(expr);

    const auto rround_token = lexer.next();
    if (rround_token.kind != TokenKind::RRoundBracket) {
        return ERR_PTR(err::SyntaxError, lexer, rround_token.location, "expected ')'");
    }

    return ast::ParenthesisExpression::alloc(std::move(expr).value());
}

util::Result<std::unique_ptr<ast::MemberExpression>> parse_member(Lexer&             lexer,
                                                                  ast::ExpressionPtr owner) {
    const auto period_token = lexer.next();
    if (period_token.kind != TokenKind::Period) {
        return ERR_PTR(err::SyntaxError, lexer, period_token.location, "expected '.'");
    }

    const auto member_token = lexer.next();
    if (member_token.kind != TokenKind::Identifier) {
        return ERR_PTR(err::SyntaxError, lexer, member_token.location,
                       "expected identifier after '.'");
    }

    return ast::MemberExpression::alloc(std::move(owner), member_token.location.substr());
}

util::Result<std::unique_ptr<ast::CallExpression>> parse_call(Lexer&             lexer,
                                                              ast::ExpressionPtr callee) {
    const auto lbracket_token = lexer.next();
    if (lbracket_token.kind != TokenKind::LRoundBracket) {
        return ERR_PTR(err::SyntaxError, lexer, lbracket_token.location, "expected '('");
    }

    std::vector<ast::ExpressionPtr> arguments;
    {
        // Handle the first argument; it's special because it doesn't have a preceding comma
        const auto next_token = lexer.peek();
        if (next_token.kind == TokenKind::RRoundBracket) {
            lexer.next();  // Consume the ')' token
            return ast::CallExpression::alloc(std::move(callee), std::move(arguments));
        }

        auto argument = parse_whole_expression(lexer);
        FORWARD_ERROR(argument);

        arguments.emplace_back(std::move(argument).value());
    }

    for (;;) {
        // Handle the rest of the arguments
        auto next_token = lexer.peek();
        switch (next_token.kind) {
            case TokenKind::Comma:
                lexer.next();  // Consume the ',' token

                if (lexer.peek().kind == TokenKind::RRoundBracket) {
                    lexer.next();  // Consume the ')' token
                    return ast::CallExpression::alloc(std::move(callee), std::move(arguments));
                }
                break;

            case TokenKind::RRoundBracket:
                lexer.next();  // Consume the ')' token
                return ast::CallExpression::alloc(std::move(callee), std::move(arguments));

            default:
                return ERR_PTR(err::SyntaxError, lexer, next_token.location, "expected ',' or ')'");
        }

        auto argument = parse_whole_expression(lexer);
        FORWARD_ERROR(argument);

        arguments.emplace_back(std::move(argument).value());
    }

    __builtin_unreachable();
}

util::Result<std::unique_ptr<ast::BlockExpression>> parse_block(Lexer& lexer) {
    const auto lbracket_token = lexer.next();
    if (lbracket_token.kind != TokenKind::LCurlyBracket) {
        return ERR_PTR(err::SyntaxError, lexer, lbracket_token.location, "expected '{'");
    }

    std::vector<ast::ExpressionPtr> expressions;
    for (;;) {
        const auto next_token = lexer.peek();
        if (next_token.kind == TokenKind::RCurlyBracket) {
            lexer.next();  // Consume the '}' token
            return ast::BlockExpression::alloc(std::move(expressions));
        }

        auto expr = parse_whole_expression(lexer);
        FORWARD_ERROR(expr);

        expressions.emplace_back(std::move(expr).value());
    }

    __builtin_unreachable();
}

util::Result<std::unique_ptr<ast::IfExpression>> parse_if(Lexer& lexer) {
    const auto token = lexer.next();
    if (token.kind != TokenKind::If) {
        return ERR_PTR(err::SyntaxError, lexer, token.location, "expected keyword 'if'");
    }

    auto condition = parse_whole_expression(lexer);
    FORWARD_ERROR(condition);

    auto then = parse_block(lexer);
    FORWARD_ERROR(then);

    std::optional<ast::ExpressionPtr> opt_else_;

    const auto else_token = lexer.peek();
    if (else_token.kind == TokenKind::Else) {
        lexer.next();  // Consume the 'else' token

        auto else_ = parse_block(lexer);
        FORWARD_ERROR(else_);

        opt_else_ = std::move(else_).value();
    }

    return ast::IfExpression::alloc(std::move(condition).value(), std::move(then).value(),
                                    std::move(opt_else_));
}

util::Result<std::unique_ptr<ast::FunctionExpression>> parse_function(Lexer& lexer) {
    const auto token = lexer.next();
    if (token.kind != TokenKind::Fn) {
        return ERR_PTR(err::SyntaxError, lexer, token.location, "expected keyword 'fn'");
    }

    std::optional<util::String> fn_name;
    auto                        next_token = lexer.next();

    // The struct name is optional
    if (next_token.kind == TokenKind::Identifier) {
        fn_name    = next_token.location.substr();
        next_token = lexer.next();
    }

    if (next_token.kind != TokenKind::LRoundBracket) {
        return ERR_PTR(err::SyntaxError, lexer, next_token.location,
                       "expected '(' before function arguments");
    }

    std::vector<ast::FunctionArgumentData> arguments;
    for (;;) {
        // Handle the function arguments
        auto next_token = lexer.next();
        if (next_token.kind == TokenKind::RRoundBracket) {
            break;
        }

        if (next_token.kind != TokenKind::Identifier) {
            return ERR_PTR(err::SyntaxError, lexer, next_token.location,
                           "expected identifier for function argument name");
        }

        const auto argument_name = next_token.location.substr();

        next_token = lexer.next();
        if (next_token.kind != TokenKind::Colon) {
            return ERR_PTR(err::SyntaxError, lexer, next_token.location,
                           "expected ':' after function argument name");
        }

        auto argument_type = parse_whole_type(lexer);
        FORWARD_ERROR(argument_type);

        arguments.emplace_back(ast::FunctionArgumentData{
            .name = argument_name,
            .type = std::move(argument_type).value(),
        });

        next_token = lexer.next();
        if (next_token.kind == TokenKind::RRoundBracket) {
            break;
        }

        if (next_token.kind != TokenKind::Comma) {
            return ERR_PTR(err::SyntaxError, lexer, next_token.location,
                           "expected ',' or ')' after function argument definition");
        }
    }

    std::optional<ast::TypePtr> return_type;
    next_token = lexer.peek();
    if (next_token.kind == TokenKind::RArrow) {
        lexer.next();  // Consume the '->' token

        auto return_type_result = parse_whole_type(lexer);
        FORWARD_ERROR(return_type_result);

        return_type = std::move(return_type_result).value();
    }

    auto body = parse_block(lexer);
    FORWARD_ERROR(body);

    return ast::FunctionExpression::alloc(std::move(fn_name), std::move(return_type),
                                          std::move(arguments),
                                          std::move(*std::move(body).value()).expressions());
}

util::Result<std::unique_ptr<ast::LetExpression>> parse_let(Lexer& lexer) {
    const auto let_token = lexer.next();
    if (let_token.kind != TokenKind::Let) {
        return ERR_PTR(err::SyntaxError, lexer, let_token.location, "expected keyword 'let'");
    }

    const auto name_token = lexer.next();
    if (name_token.kind != TokenKind::Identifier) {
        return ERR_PTR(err::SyntaxError, lexer, name_token.location,
                       "expected variable name after 'let'");
    }
    const auto let_name = name_token.location.substr();

    const auto eq_token = lexer.next();
    if (eq_token.kind != TokenKind::Equal) {
        return ERR_PTR(err::SyntaxError, lexer, eq_token.location,
                       "expected '=' between variable name and initial value");
    }

    auto value = parse_whole_expression(lexer);
    FORWARD_ERROR(value);

    // TODO: Handle the 'mutable' keyword
    bool mutable_ = true;
    return ast::LetExpression::alloc(std::move(let_name), std::move(value).value(), mutable_);
}

util::Result<std::unique_ptr<ast::ReturnExpression>> parse_return(Lexer& lexer) {
    const auto token = lexer.next();
    if (token.kind != TokenKind::Return) {
        return ERR_PTR(err::SyntaxError, lexer, token.location, "expected keyword 'return'");
    }

    auto expr = parse_whole_expression(lexer);
    FORWARD_ERROR(expr);

    return ast::ReturnExpression::alloc(std::move(expr).value());
}

util::Result<ast::ExpressionPtr>                   parse_lhs(Lexer& lexer);
util::Result<std::unique_ptr<ast::ExecExpression>> parse_exec(Lexer& lexer) {
    const auto token = lexer.next();
    if (token.kind != TokenKind::Hash) {
        return ERR_PTR(err::SyntaxError, lexer, token.location, "expected '#'");
    }

    auto expr = parse_lhs(lexer);
    FORWARD_ERROR(expr);

    return ast::ExecExpression::alloc(std::move(expr).value());
}

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

util::Result<ast::ExpressionPtr> parse_lhs(Lexer& lexer) {
    util::Result<ast::ExpressionPtr> expression;

    const auto token = lexer.peek();
    switch (token.kind) {
        case TokenKind::Integer:
            expression = parse_integer(lexer);
            break;

        case TokenKind::Float:
            expression = parse_float(lexer);
            break;

        case TokenKind::Identifier:
            expression = parse_identifier(lexer);
            break;

        case TokenKind::LRoundBracket:
            expression = parse_parenthesis(lexer);
            break;

        case TokenKind::LCurlyBracket:
            expression = parse_block(lexer);
            break;

        case TokenKind::If:
            expression = parse_if(lexer);
            break;

        case TokenKind::Fn:
            expression = parse_function(lexer);
            break;

        case TokenKind::Hash:
            expression = parse_exec(lexer);
            break;

        case TokenKind::Eof:
            return ERR_PTR(err::SyntaxError, lexer, token.location, "unexpected end of file");

        default:
            return ERR_PTR(err::SyntaxError, lexer, token.location, "unexpected token");
    }
    FORWARD_ERROR(expression);

    auto next_token = lexer.peek();
    while (next_token.kind == TokenKind::Period || next_token.kind == TokenKind::LRoundBracket) {
        if (next_token.kind == TokenKind::Period) {
            expression = parse_member(lexer, std::move(expression).value());
            FORWARD_ERROR(expression);

            next_token = lexer.peek();
            continue;
        }

        if (next_token.kind == TokenKind::LRoundBracket) {
            expression = parse_call(lexer, std::move(expression).value());
            FORWARD_ERROR(expression);

            next_token = lexer.peek();
            continue;
        }
    }

    return expression;
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

        auto rhs_expression = parse_lhs(lexer);
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

util::Result<ast::ExpressionPtr> parse_whole_expression(Lexer& lexer) {
    // Handle the expressions that are only allowed at the top level
    const auto token = lexer.peek();
    switch (token.kind) {
        case TokenKind::Let:
            return parse_let(lexer);

        case TokenKind::Return:
            return parse_return(lexer);

        default:
            // Default to parsing any type of expression that can be combined with operators
            auto lhs = parse_lhs(lexer);
            FORWARD_ERROR(lhs);

            return parse_rhs(lexer, std::move(lhs).value(), Precendence::Unknown);
    }
}

util::Result<std::unique_ptr<ast::ExportExpression>> parse_export(Lexer& lexer) {
    const auto export_token = lexer.next();
    if (export_token.kind != TokenKind::Export) {
        return ERR_PTR(err::SyntaxError, lexer, export_token.location, "expected keyword 'export'");
    }

    const auto next_token = lexer.peek();
    switch (next_token.kind) {
        case TokenKind::Fn: {
            auto result = parse_function(lexer);
            FORWARD_ERROR(result);
            return ast::ExportExpression::alloc(std::move(result).value());
        }

        default:
            return ERR_PTR(err::SyntaxError, lexer, next_token.location, "unexpected token");
    }
}

}  // namespace

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
                return ERR_PTR(err::SyntaxError, lexer, token.location, "unexpected token");
            }
        }
    }

    __builtin_unreachable();
}

}  // namespace cirrus::lang
