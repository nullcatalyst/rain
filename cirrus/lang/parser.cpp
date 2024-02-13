#include "cirrus/lang/parser.hpp"

#include <array>
#include <iostream>
#include <sstream>

#include "cirrus/ast/expr/all.hpp"
#include "cirrus/ast/type/all.hpp"
#include "cirrus/err/all.hpp"
#include "cirrus/util/colors.hpp"
#include "cirrus/util/result.hpp"

namespace cirrus::lang {

namespace {

util::Result<ast::Type> parse_whole_type(Lexer& lexer);

util::Result<ast::StructType> parse_struct(Lexer& lexer) {
    const auto struct_token = lexer.next();
    if (struct_token.kind != TokenKind::Struct) {
        return ERR_PTR(ast::StructType, err::SyntaxError, lexer, struct_token.location,
                       "expected keyword 'struct'");
    }

    std::optional<util::String> struct_name;
    auto                        next_token = lexer.next();

    // The struct name is optional
    if (next_token.kind == TokenKind::Identifier) {
        struct_name = next_token.location.substr();
        next_token  = lexer.next();
    }

    if (next_token.kind != TokenKind::LCurlyBracket) {
        return ERR_PTR(ast::StructType, err::SyntaxError, lexer, next_token.location,
                       "expected '{' after struct identifier");
    }

    std::vector<ast::StructTypeFieldData> struct_fields;
    for (;;) {
        // Handle the struct fields
        auto next_token = lexer.next();

        switch (next_token.kind) {
            case TokenKind::RCurlyBracket:
                return OK_ALLOC(ast::StructType, std::move(struct_name), std::move(struct_fields));

            case TokenKind::Identifier:
                // Found a field name
                break;

            default:
                return ERR_PTR(ast::StructType, err::SyntaxError, lexer, next_token.location,
                               "expected identifier for struct field name");
        }

        const auto field_name = next_token.location.substr();

        next_token = lexer.next();
        if (next_token.kind != TokenKind::Colon) {
            return ERR_PTR(ast::StructType, err::SyntaxError, lexer, next_token.location,
                           "expected ':' after struct field name");
        }

        auto field_type = parse_whole_type(lexer);
        FORWARD_ERROR_WITH_TYPE(ast::StructType, field_type);

        struct_fields.emplace_back(ast::StructTypeFieldData{
            .name = field_name,
            .type = std::move(field_type.unwrap()),
        });

        next_token = lexer.next();
        if (next_token.kind != TokenKind::Semicolon) {
            return ERR_PTR(ast::StructType, err::SyntaxError, lexer, next_token.location,
                           "expected ';' after struct field definition");
        }
    }

    __builtin_unreachable();
}

util::Result<ast::Type> parse_whole_type(Lexer& lexer) {
    const auto token = lexer.peek();
    switch (token.kind) {
        case TokenKind::Struct:
            return parse_struct(lexer);

        case TokenKind::Identifier:
            lexer.next();  // Consume the identifier token
            return OK_ALLOC(ast::UnresolvedType, token.location.substr());

        default:
            return ERR_PTR(ast::Type, err::SyntaxError, lexer, token.location, "unexpected token");
    }
}

util::Result<ast::Expression> parse_whole_expression(Lexer& lexer);

util::Result<ast::IntegerExpression> parse_integer(Lexer& lexer) {
    const auto token = lexer.next();
    if (token.kind != TokenKind::Integer) {
        return ERR_PTR(ast::IntegerExpression, err::SyntaxError, lexer, token.location,
                       "expected integer literal");
    }

    constexpr uint64_t MAX   = std::numeric_limits<uint64_t>::max() / 10;
    uint64_t           value = 0;

    // TODO: Add support for util::Twine for each loops
    // for (char c : token.location.source.substr()) {
    const auto s = token.location.substr();
    for (auto begin = 0, end = s.size(); begin < end; ++begin) {
        const char c = s[begin];
        if (value > MAX) {
            // Multiplying by 10 will overflow
            return ERR_PTR(ast::IntegerExpression, err::SyntaxError, lexer, token.location,
                           "integer literal is too large");
        }
        value *= 10;

        if (value > std::numeric_limits<uint64_t>::max() - (c - '0')) {
            // Adding the next digit will overflow
            return ERR_PTR(ast::IntegerExpression, err::SyntaxError, lexer, token.location,
                           "integer literal is too large");
        }
        value += c - '0';
    }

    return OK_ALLOC(ast::IntegerExpression, value);
}

util::Result<ast::IdentifierExpression> parse_identifier(Lexer& lexer) {
    const auto token = lexer.next();
    if (token.kind != TokenKind::Identifier) {
        return ERR_PTR(ast::IdentifierExpression, err::SyntaxError, lexer, token.location,
                       "expected identifier");
    }

    return OK_ALLOC(ast::IdentifierExpression, token.location.substr());
}

util::Result<ast::ParenthesisExpression> parse_parenthesis(Lexer& lexer) {
    const auto lround_token = lexer.next();
    if (lround_token.kind != TokenKind::LRoundBracket) {
        return ERR_PTR(ast::ParenthesisExpression, err::SyntaxError, lexer, lround_token.location,
                       "expected '('");
    }

    auto expr = parse_whole_expression(lexer);
    FORWARD_ERROR_WITH_TYPE(ast::ParenthesisExpression, expr);

    const auto rround_token = lexer.next();
    if (rround_token.kind != TokenKind::RRoundBracket) {
        return ERR_PTR(ast::ParenthesisExpression, err::SyntaxError, lexer, rround_token.location,
                       "expected ')'");
    }

    return OK_ALLOC(ast::ParenthesisExpression, expr.unwrap());
}

util::Result<ast::MemberExpression> parse_member(Lexer& lexer, ast::Expression owner) {
    const auto period_token = lexer.next();
    if (period_token.kind != TokenKind::Period) {
        return ERR_PTR(ast::MemberExpression, err::SyntaxError, lexer, period_token.location,
                       "expected '.'");
    }

    const auto member_token = lexer.next();
    if (member_token.kind != TokenKind::Identifier) {
        return ERR_PTR(ast::MemberExpression, err::SyntaxError, lexer, member_token.location,
                       "expected identifier after '.'");
    }

    return OK_ALLOC(ast::MemberExpression, std::move(owner), member_token.location.substr());
}

util::Result<ast::CallExpression> parse_call(Lexer& lexer, ast::Expression callee) {
    const auto lbracket_token = lexer.next();
    if (lbracket_token.kind != TokenKind::LRoundBracket) {
        return ERR_PTR(ast::CallExpression, err::SyntaxError, lexer, lbracket_token.location,
                       "expected '('");
    }

    std::vector<ast::Expression> arguments;
    {
        // Handle the first argument; it's special because it doesn't have a preceding comma
        const auto next_token = lexer.peek();
        if (next_token.kind == TokenKind::RRoundBracket) {
            lexer.next();  // Consume the ')' token
            return OK_ALLOC(ast::CallExpression, std::move(callee), std::move(arguments));
        }

        auto argument = parse_whole_expression(lexer);
        FORWARD_ERROR_WITH_TYPE(ast::CallExpression, argument);

        arguments.emplace_back(std::move(argument.unwrap()));
    }

    for (;;) {
        // Handle the rest of the arguments
        auto next_token = lexer.peek();
        switch (next_token.kind) {
            case TokenKind::Comma:
                lexer.next();  // Consume the ',' token

                if (lexer.peek().kind == TokenKind::RRoundBracket) {
                    lexer.next();  // Consume the ')' token
                    return OK_ALLOC(ast::CallExpression, std::move(callee), std::move(arguments));
                }
                break;

            case TokenKind::RRoundBracket:
                lexer.next();  // Consume the ')' token
                return OK_ALLOC(ast::CallExpression, std::move(callee), std::move(arguments));

            default:
                return ERR_PTR(ast::CallExpression, err::SyntaxError, lexer, next_token.location,
                               "expected ',' or ')'");
        }

        auto argument = parse_whole_expression(lexer);
        FORWARD_ERROR_WITH_TYPE(ast::CallExpression, argument);

        arguments.emplace_back(std::move(argument.unwrap()));
    }

    __builtin_unreachable();
}

util::Result<ast::BlockExpression> parse_block(Lexer& lexer) {
    const auto lbracket_token = lexer.next();
    if (lbracket_token.kind != TokenKind::LCurlyBracket) {
        return ERR_PTR(ast::BlockExpression, err::SyntaxError, lexer, lbracket_token.location,
                       "expected '{'");
    }

    std::vector<ast::Expression> expressions;
    for (;;) {
        const auto next_token = lexer.peek();
        if (next_token.kind == TokenKind::RCurlyBracket) {
            lexer.next();  // Consume the '}' token
            return OK_ALLOC(ast::BlockExpression, std::move(expressions));
        }

        auto expr = parse_whole_expression(lexer);
        FORWARD_ERROR_WITH_TYPE(ast::BlockExpression, expr);

        expressions.emplace_back(std::move(expr.unwrap()));
    }

    __builtin_unreachable();
}

util::Result<ast::IfExpression> parse_if(Lexer& lexer) {
    const auto token = lexer.next();
    if (token.kind != TokenKind::If) {
        return ERR_PTR(ast::IfExpression, err::SyntaxError, lexer, token.location,
                       "expected keyword 'if'");
    }

    auto condition = parse_whole_expression(lexer);
    FORWARD_ERROR_WITH_TYPE(ast::IfExpression, condition);

    auto then = parse_block(lexer);
    FORWARD_ERROR_WITH_TYPE(ast::IfExpression, then);

    std::optional<ast::Expression> opt_else_ = std::nullopt;

    const auto else_token = lexer.peek();
    if (else_token.kind == TokenKind::Else) {
        lexer.next();  // Consume the 'else' token

        auto else_ = parse_block(lexer);
        FORWARD_ERROR_WITH_TYPE(ast::IfExpression, else_);

        opt_else_ = std::move(else_.unwrap());
    }

    return OK_ALLOC(ast::IfExpression, std::move(condition.unwrap()), std::move(then.unwrap()),
                    std::move(opt_else_));
}

util::Result<ast::FunctionExpression> parse_function(Lexer& lexer) {
    const auto token = lexer.next();
    if (token.kind != TokenKind::Fn) {
        return ERR_PTR(ast::FunctionExpression, err::SyntaxError, lexer, token.location,
                       "expected keyword 'fn'");
    }

    std::optional<util::String> fn_name;
    auto                        next_token = lexer.next();

    // The struct name is optional
    if (next_token.kind == TokenKind::Identifier) {
        fn_name    = next_token.location.substr();
        next_token = lexer.next();
    }

    if (next_token.kind != TokenKind::LRoundBracket) {
        return ERR_PTR(ast::FunctionExpression, err::SyntaxError, lexer, next_token.location,
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
            return ERR_PTR(ast::FunctionExpression, err::SyntaxError, lexer, next_token.location,
                           "expected identifier for function argument name");
        }

        const auto argument_name = next_token.location.substr();

        next_token = lexer.next();
        if (next_token.kind != TokenKind::Colon) {
            return ERR_PTR(ast::FunctionExpression, err::SyntaxError, lexer, next_token.location,
                           "expected ':' after function argument name");
        }

        auto argument_type = parse_whole_type(lexer);
        FORWARD_ERROR_WITH_TYPE(ast::FunctionExpression, argument_type);

        arguments.emplace_back(ast::FunctionArgumentData{
            .name = argument_name,
            .type = std::move(argument_type.unwrap()),
        });

        next_token = lexer.next();
        if (next_token.kind == TokenKind::RRoundBracket) {
            break;
        }

        if (next_token.kind != TokenKind::Comma) {
            return ERR_PTR(ast::FunctionExpression, err::SyntaxError, lexer, next_token.location,
                           "expected ',' or ')' after function argument definition");
        }
    }

    std::optional<ast::Type> return_type;
    next_token = lexer.peek();
    if (next_token.kind == TokenKind::RArrow) {
        lexer.next();  // Consume the '->' token

        auto return_type_result = parse_whole_type(lexer);
        FORWARD_ERROR_WITH_TYPE(ast::FunctionExpression, return_type_result);

        return_type = std::move(return_type_result.unwrap());
    }

    auto body = parse_block(lexer);
    FORWARD_ERROR_WITH_TYPE(ast::FunctionExpression, body);

    return OK_ALLOC(ast::FunctionExpression, std::move(fn_name), std::move(return_type),
                    std::move(arguments), std::move(body.unwrap().expressions()));
}

util::Result<ast::LetExpression> parse_let(Lexer& lexer) {
    const auto let_token = lexer.next();
    if (let_token.kind != TokenKind::Let) {
        return ERR_PTR(ast::LetExpression, err::SyntaxError, lexer, let_token.location,
                       "expected keyword 'let'");
    }

    const auto name_token = lexer.next();
    if (name_token.kind != TokenKind::Identifier) {
        return ERR_PTR(ast::LetExpression, err::SyntaxError, lexer, name_token.location,
                       "expected variable name after 'let'");
    }
    const auto let_name = name_token.location.substr();

    const auto eq_token = lexer.next();
    if (eq_token.kind != TokenKind::Equal) {
        return ERR_PTR(ast::LetExpression, err::SyntaxError, lexer, eq_token.location,
                       "expected '=' between variable name and initial value");
    }

    auto value = parse_whole_expression(lexer);
    FORWARD_ERROR_WITH_TYPE(ast::LetExpression, value);

    // TODO: Handle the 'mutable' keyword
    bool mutable_ = true;
    return OK_ALLOC(ast::LetExpression, std::move(let_name), std::move(value.unwrap()), mutable_);
}

util::Result<ast::ReturnExpression> parse_return(Lexer& lexer) {
    const auto token = lexer.next();
    if (token.kind != TokenKind::Return) {
        return ERR_PTR(ast::ReturnExpression, err::SyntaxError, lexer, token.location,
                       "expected keyword 'return'");
    }

    auto expr = parse_whole_expression(lexer);
    FORWARD_ERROR_WITH_TYPE(ast::ReturnExpression, expr);

    return OK_ALLOC(ast::ReturnExpression, std::move(expr.unwrap()));
}

util::Result<ast::Expression>     parse_lhs(Lexer& lexer);
util::Result<ast::ExecExpression> parse_exec(Lexer& lexer) {
    const auto token = lexer.next();
    if (token.kind != TokenKind::Hash) {
        return ERR_PTR(ast::ExecExpression, err::SyntaxError, lexer, token.location,
                       "expected '#'");
    }

    auto expr = parse_lhs(lexer);
    FORWARD_ERROR_WITH_TYPE(ast::ExecExpression, expr);

    return OK_ALLOC(ast::ExecExpression, std::move(expr.unwrap()));
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

ast::BinaryOperator get_operator_for_token(Token token) {
    switch (token.kind) {
        case TokenKind::Plus:
            return ast::BinaryOperator::Add;
        case TokenKind::Minus:
            return ast::BinaryOperator::Subtract;
        case TokenKind::Star:
            return ast::BinaryOperator::Multiply;
        case TokenKind::Slash:
            return ast::BinaryOperator::Divide;
        case TokenKind::Percent:
            return ast::BinaryOperator::Modulo;
        case TokenKind::Ampersand:
            return ast::BinaryOperator::And;
        case TokenKind::Pipe:
            return ast::BinaryOperator::Or;
        case TokenKind::Caret:
            return ast::BinaryOperator::Xor;
        case TokenKind::LessLess:
            return ast::BinaryOperator::ShiftLeft;
        case TokenKind::GreaterGreater:
            return ast::BinaryOperator::ShiftRight;
        case TokenKind::EqualEqual:
            return ast::BinaryOperator::Equal;
        case TokenKind::ExclaimEqual:
            return ast::BinaryOperator::NotEqual;
        case TokenKind::Less:
            return ast::BinaryOperator::Less;
        case TokenKind::Greater:
            return ast::BinaryOperator::Greater;
        case TokenKind::LessEqual:
            return ast::BinaryOperator::LessEqual;
        case TokenKind::GreaterEqual:
            return ast::BinaryOperator::GreaterEqual;
        default:
            return ast::BinaryOperator::Unknown;
    }
}

util::Result<ast::Expression> parse_lhs(Lexer& lexer) {
    util::Result<ast::Expression> expression;

    const auto token = lexer.peek();
    switch (token.kind) {
        case TokenKind::Integer:
            expression = parse_integer(lexer);
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
            return ERR_PTR(ast::Expression, err::SyntaxError, lexer, token.location,
                           "unexpected end of file");

        default:
            return ERR_PTR(ast::Expression, err::SyntaxError, lexer, token.location,
                           "unexpected token");
    }
    FORWARD_ERROR(expression);

    auto next_token = lexer.peek();
    while (next_token.kind == TokenKind::Period || next_token.kind == TokenKind::LRoundBracket) {
        if (next_token.kind == TokenKind::Period) {
            expression = parse_member(lexer, expression.unwrap());
            FORWARD_ERROR(expression);

            next_token = lexer.peek();
            continue;
        }

        if (next_token.kind == TokenKind::LRoundBracket) {
            expression = parse_call(lexer, expression.unwrap());
            FORWARD_ERROR(expression);

            next_token = lexer.peek();
            continue;
        }
    }

    return expression;
}

util::Result<ast::Expression> parse_rhs(Lexer& lexer, ast::Expression lhs_expression,
                                        int lhs_precedence) {
    for (;;) {
        const auto binop_token = lexer.peek();
        const auto binop       = get_operator_for_token(binop_token);
        if (binop == ast::BinaryOperator::Unknown) {
            return util::Result<ast::Expression>::ok(std::move(lhs_expression));
        }

        int binop_precedence = BINARY_OPERATOR_PRECEDENCE[static_cast<int>(binop)];
        if (binop_precedence < lhs_precedence) {
            return util::Result<ast::Expression>::ok(std::move(lhs_expression));
        }
        lexer.next();  // Consume the operator

        auto rhs_expression = parse_lhs(lexer);
        FORWARD_ERROR(rhs_expression);

        const auto next_token = lexer.peek();
        const auto next_binop = get_operator_for_token(next_token);
        if (next_binop != ast::BinaryOperator::Unknown) {
            const auto rhs_precedence = BINARY_OPERATOR_PRECEDENCE[static_cast<int>(next_binop)];
            if (binop_precedence < rhs_precedence) {
                rhs_expression =
                    parse_rhs(lexer, std::move(rhs_expression.unwrap()), binop_precedence);
                FORWARD_ERROR(rhs_expression);
            }
        }

        lhs_expression = ast::BinaryOperatorExpression::alloc(
            std::move(lhs_expression), std::move(rhs_expression.unwrap()), binop);
    }
}

util::Result<ast::Expression> parse_whole_expression(Lexer& lexer) {
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

            return parse_rhs(lexer, lhs.unwrap(), Precendence::Unknown);
    }
}

util::Result<ast::ExportExpression> parse_export(Lexer& lexer) {
    const auto export_token = lexer.next();
    if (export_token.kind != TokenKind::Export) {
        return ERR_PTR(ast::ExportExpression, err::SyntaxError, lexer, export_token.location,
                       "expected keyword 'export'");
    }

    const auto next_token = lexer.peek();
    switch (next_token.kind) {
        case TokenKind::Fn: {
            auto result = parse_function(lexer);
            FORWARD_ERROR_WITH_TYPE(ast::ExportExpression, result);
            return OK_ALLOC(ast::ExportExpression, std::move(result.unwrap()));
        }

        default:
            return ERR_PTR(ast::ExportExpression, err::SyntaxError, lexer, next_token.location,
                           "unexpected token");
    }
}

}  // namespace

util::Result<ast::Type> Parser::parse_type(Lexer& lexer) { return parse_whole_type(lexer); }

util::Result<ast::Expression> Parser::parse_expression(Lexer& lexer) {
    return parse_whole_expression(lexer);
}

util::Result<ParsedModule> Parser::parse(Lexer& lexer) {
    std::vector<ast::Node> nodes;

    for (;;) {
        const auto token = lexer.peek();
        switch (token.kind) {
            case TokenKind::Struct: {
                auto result = parse_type(lexer);
                FORWARD_ERROR_WITH_TYPE(ParsedModule, result);
                nodes.emplace_back(std::move(result.unwrap()));
                break;
            }

            case TokenKind::Export: {
                auto result = parse_export(lexer);
                FORWARD_ERROR_WITH_TYPE(ParsedModule, result);
                nodes.emplace_back(std::move(result.unwrap()));
                break;
            }

            case TokenKind::Fn: {
                auto result = parse_function(lexer);
                FORWARD_ERROR_WITH_TYPE(ParsedModule, result);
                nodes.emplace_back(std::move(result.unwrap()));
                break;
            }

            case TokenKind::Eof: {
                return OK(ParsedModule, std::move(nodes));
            }

            default: {
                return ERR_PTR(ParsedModule, err::SyntaxError, lexer, token.location,
                               "unexpected token");
            }
        }
    }

    __builtin_unreachable();
}

}  // namespace cirrus::lang
