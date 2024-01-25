#include "cirrus/lang/parser.hpp"

#include <iostream>
#include <sstream>

#include "cirrus/ast/expr/all.hpp"
#include "cirrus/ast/type/all.hpp"
#include "cirrus/util/result.hpp"

namespace cirrus::lang {

namespace {

#if 1
constexpr std::string_view ANSI_RESET = "\033[0m";
constexpr std::string_view ANSI_RED   = "\033[31m";
constexpr std::string_view ANSI_GREEN = "\033[32m";
constexpr std::string_view ANSI_BLUE  = "\033[34m";
constexpr std::string_view ANSI_BOLD  = "\033[1m";
#else
constexpr std::string_view ANSI_RESET = "";
constexpr std::string_view ANSI_RED   = "";
constexpr std::string_view ANSI_GREEN = "";
constexpr std::string_view ANSI_BLUE  = "";
constexpr std::string_view ANSI_BOLD  = "";
#endif

class SyntaxError : public util::Error {
    const Lexer& _lexer;
    Location     _location;
    std::string  _msg;

  public:
    SyntaxError(const Lexer& lexer, Location location, std::string msg)
        : _lexer(lexer), _location(location), _msg(std::move(msg)) {}
    ~SyntaxError() override = default;

    [[nodiscard]] std::string message() const noexcept override {
        std::stringstream ss;

        ss << ANSI_BOLD << _lexer.file_name() << ':' << _location.line << ':' << _location.column
           << ": " << ANSI_RED << "error: " << ANSI_RESET << ANSI_BOLD << _msg << ANSI_RESET
           << '\n';
        ss << _lexer.get_whole_line(_location).source << '\n';

        for (int i = 0; i < _location.column - 1; ++i) {
            ss << ' ';
        }
        ss << ANSI_GREEN << '^';
        for (int i = 0; i < _location.source.size() - 1; ++i) {
            ss << '~';
        }
        ss << ANSI_RESET << '\n';

        return ss.str();
    }
};

class BinaryOperatorError : public util::Error {
    const Lexer& _lexer;
    Location     _lhs_location;
    Location     _rhs_location;
    Location     _op_location;
    std::string  _msg;

  public:
    BinaryOperatorError(const Lexer& lexer, Location lhs_location, Location rhs_location,
                        Location op_location, std::string msg)
        : _lexer(lexer),
          _lhs_location(lhs_location),
          _rhs_location(rhs_location),
          _op_location(op_location),
          _msg(std::move(msg)) {}
    ~BinaryOperatorError() override = default;

    [[nodiscard]] std::string message() const noexcept override {
        std::stringstream ss;

        ss << ANSI_BOLD << _lexer.file_name() << ":" << _op_location.line << ":"
           << _op_location.column << ": " << ANSI_RED << "error: " << ANSI_RESET << ANSI_BOLD
           << _msg << ANSI_RESET << "\n";
        ss << _lexer.get_whole_line(_op_location).source << "\n";

        int column = 1;
        if (_lhs_location.line == _op_location.line) {
            // Underline the LHS expression
            for (; column < _lhs_location.column; ++column) {
                ss << ' ';
            }

            ss << ANSI_GREEN;
            for (int i = 0; i < _lhs_location.source.size() - 1; ++i, ++column) {
                ss << '~';
            }
            ss << ANSI_RESET;
        }

        {
            // Mark the operator with a caret
            for (; column < _op_location.column; ++column) {
                ss << ' ';
            }
            ss << ANSI_GREEN << '^' << ANSI_RESET;
            ++column;
        }

        if (_rhs_location.line == _op_location.line) {
            // Underline the RHS expression
            for (; column < _rhs_location.column; ++column) {
                ss << ' ';
            }

            ss << ANSI_GREEN;
            for (int i = 0; i < _rhs_location.source.size() - 1; ++i, ++column) {
                ss << '~';
            }
            ss << ANSI_RESET << '\n';
        }

        return ss.str();
    }
};

// Some macros to shorten the error creation

#define CREATE_OK(ResultType, ...) util::Result<ResultType>::ok(ResultType::alloc(__VA_ARGS__))

#define CREATE_ERROR(ResultType, ErrorType, ...) \
    util::Result<ResultType>::error(std::make_unique<ErrorType>(__VA_ARGS__))

#define CREATE_SYNTAX_ERROR(ResultType, lexer, location, msg) \
    CREATE_ERROR(ResultType, SyntaxError, lexer, location, msg)

#define CREATE_BINARY_OPERATOR_ERROR(ResultType, lexer, lhs_location, rhs_location, op_location,  \
                                     msg)                                                         \
    CREATE_ERROR(ResultType, BinaryOperatorError, lexer, lhs_location, rhs_location, op_location, \
                 msg)

util::Result<ast::Type> parse_whole_type(Lexer& lexer);

util::Result<ast::StructType> parse_struct(Lexer& lexer) {
    const auto struct_token = lexer.next();
    if (struct_token.kind != TokenKind::Struct) {
        return CREATE_SYNTAX_ERROR(ast::StructType, lexer, struct_token.location,
                                   "expected keyword 'struct'");
    }

    std::optional<std::string_view> struct_name;
    auto                            next_token = lexer.next();

    // The struct name is optional
    if (next_token.kind == TokenKind::Identifier) {
        struct_name = next_token.location.source;
        next_token  = lexer.next();
    }

    if (next_token.kind != TokenKind::LCurlyBracket) {
        return CREATE_SYNTAX_ERROR(ast::StructType, lexer, next_token.location,
                                   "expected '{' after struct identifier");
    }

    std::vector<ast::StructTypeFieldData> struct_fields;
    for (;;) {
        // Handle the struct fields
        auto next_token = lexer.next();

        switch (next_token.kind) {
            case TokenKind::RCurlyBracket:
                return CREATE_OK(ast::StructType, std::move(struct_name), std::move(struct_fields));

            case TokenKind::Identifier:
                // Found a field name
                break;

            default:
                return CREATE_SYNTAX_ERROR(ast::StructType, lexer, next_token.location,
                                           "expected identifier for struct field name");
        }

        const auto field_name = next_token.location.source;

        next_token = lexer.next();
        if (next_token.kind != TokenKind::Colon) {
            return CREATE_SYNTAX_ERROR(ast::StructType, lexer, next_token.location,
                                       "expected ':' after struct field name");
        }

        auto field_type = parse_whole_type(lexer);
        if (field_type.is_error()) {
            return util::Result<ast::StructType>::error(std::move(field_type.unwrap_error()));
        }

        struct_fields.emplace_back(ast::StructTypeFieldData{
            .name = field_name,
            .type = std::move(field_type.unwrap()),
        });

        next_token = lexer.next();
        if (next_token.kind != TokenKind::Semicolon) {
            return CREATE_SYNTAX_ERROR(ast::StructType, lexer, next_token.location,
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
            return CREATE_OK(ast::UnresolvedType, token.location.source);

        default:
            return CREATE_SYNTAX_ERROR(ast::Type, lexer, token.location, "unexpected token");
    }
}

util::Result<ast::Expression> parse_whole_expression(Lexer& lexer);

util::Result<ast::IntegerExpression> parse_integer(Lexer& lexer) {
    const auto token = lexer.next();
    if (token.kind != TokenKind::Integer) {
        return CREATE_SYNTAX_ERROR(ast::IntegerExpression, lexer, token.location,
                                   "expected integer literal");
    }

    constexpr uint64_t MAX   = std::numeric_limits<uint64_t>::max() / 10;
    uint64_t           value = 0;
    for (char c : token.location.source) {
        if (value > MAX) {
            // Multiplying by 10 will overflow
            return CREATE_SYNTAX_ERROR(ast::IntegerExpression, lexer, token.location,
                                       "integer literal is too large");
        }
        value *= 10;

        if (value > std::numeric_limits<uint64_t>::max() - (c - '0')) {
            // Adding the next digit will overflow
            return CREATE_SYNTAX_ERROR(ast::IntegerExpression, lexer, token.location,
                                       "integer literal is too large");
        }
        value += c - '0';
    }

    return CREATE_OK(ast::IntegerExpression, value);
}

util::Result<ast::IdentifierExpression> parse_identifier(Lexer& lexer) {
    const auto token = lexer.next();
    if (token.kind != TokenKind::Identifier) {
        return CREATE_SYNTAX_ERROR(ast::IdentifierExpression, lexer, token.location,
                                   "expected identifier");
    }

    return CREATE_OK(ast::IdentifierExpression, token.location.source);
}

util::Result<ast::ParenthesisExpression> parse_parenthesis(Lexer& lexer) {
    const auto lround_token = lexer.next();
    if (lround_token.kind != TokenKind::LRoundBracket) {
        return CREATE_SYNTAX_ERROR(ast::ParenthesisExpression, lexer, lround_token.location,
                                   "expected '('");
    }

    auto expr_result = parse_whole_expression(lexer);
    if (expr_result.is_error()) {
        return util::Result<ast::ParenthesisExpression>::error(
            std::move(expr_result.unwrap_error()));
    }

    const auto rround_token = lexer.next();
    if (rround_token.kind != TokenKind::RRoundBracket) {
        return CREATE_SYNTAX_ERROR(ast::ParenthesisExpression, lexer, rround_token.location,
                                   "expected ')'");
    }

    return CREATE_OK(ast::ParenthesisExpression, expr_result.unwrap());
}

util::Result<ast::Expression> parse_call(Lexer& lexer, ast::Expression callee) {
    const auto lbracket_token = lexer.next();
    if (lbracket_token.kind != TokenKind::LRoundBracket) {
        return CREATE_SYNTAX_ERROR(ast::Expression, lexer, lbracket_token.location, "expected '('");
    }

    std::vector<ast::Expression> arguments;
    {
        // Handle the first argument; it's special because it doesn't have a preceding comma
        const auto next_token = lexer.peek();
        if (next_token.kind == TokenKind::RRoundBracket) {
            lexer.next();  // Consume the ')' token
            return CREATE_OK(ast::CallExpression, std::move(callee), std::move(arguments));
        }

        auto argument = parse_whole_expression(lexer);
        if (argument.is_error()) {
            return argument;
        }

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
                    return CREATE_OK(ast::CallExpression, std::move(callee), std::move(arguments));
                }
                break;

            case TokenKind::RRoundBracket:
                lexer.next();  // Consume the ')' token
                return CREATE_OK(ast::CallExpression, std::move(callee), std::move(arguments));

            default:
                return CREATE_SYNTAX_ERROR(ast::Expression, lexer, next_token.location,
                                           "expected ',' or ')'");
        }

        auto argument = parse_whole_expression(lexer);
        if (argument.is_error()) {
            return argument;
        }

        arguments.emplace_back(std::move(argument.unwrap()));
    }

    __builtin_unreachable();
}

util::Result<ast::Expression> parse_block(Lexer& lexer) {
    const auto lbracket_token = lexer.next();
    if (lbracket_token.kind != TokenKind::LCurlyBracket) {
        return CREATE_SYNTAX_ERROR(ast::Expression, lexer, lbracket_token.location, "expected '{'");
    }

    std::vector<ast::Expression> expressions;
    for (;;) {
        const auto next_token = lexer.peek();
        if (next_token.kind == TokenKind::RCurlyBracket) {
            lexer.next();  // Consume the '}' token
            break;
        }

        auto expr_result = parse_whole_expression(lexer);
        if (expr_result.is_error()) {
            return expr_result;
        }

        expressions.emplace_back(std::move(expr_result.unwrap()));
    }

    return CREATE_OK(ast::BlockExpression, std::move(expressions));
}

util::Result<ast::Expression> parse_if(Lexer& lexer) {
    const auto token = lexer.next();
    if (token.kind != TokenKind::If) {
        return CREATE_SYNTAX_ERROR(ast::Expression, lexer, token.location, "expected keyword 'if'");
    }

    auto condition = parse_whole_expression(lexer);
    if (condition.is_error()) {
        return condition;
    }

    auto then = parse_block(lexer);
    if (then.is_error()) {
        return then;
    }

    std::optional<ast::Expression> else_ = std::nullopt;

    const auto else_token = lexer.peek();
    if (else_token.kind == TokenKind::Else) {
        lexer.next();  // Consume the 'else' token

        auto else_result = parse_block(lexer);
        if (else_result.is_error()) {
            return else_result;
        }

        else_ = std::move(else_result.unwrap());
    }

    return CREATE_OK(ast::IfExpression, std::move(condition.unwrap()), std::move(then.unwrap()),
                     std::move(else_));
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
        // case TokenKind::Ampersand:
        //     return ast::BinaryOperator::And;
        // case TokenKind::Pipe:
        //     return ast::BinaryOperator::Or;
        // case TokenKind::Caret:
        //     return ast::BinaryOperator::Xor;
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

        default:
            return CREATE_SYNTAX_ERROR(ast::Expression, lexer, token.location, "unexpected token");
    }

    if (expression.is_error()) {
        return expression;
    }

    if (lexer.peek().kind == TokenKind::LRoundBracket) {
        expression = parse_call(lexer, expression.unwrap());
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

        auto rhs_result = parse_lhs(lexer);
        if (rhs_result.is_error()) {
            return rhs_result;
        }

        const auto next_token = lexer.peek();
        const auto next_binop = get_operator_for_token(next_token);
        if (next_binop != ast::BinaryOperator::Unknown) {
            const auto rhs_precedence = BINARY_OPERATOR_PRECEDENCE[static_cast<int>(next_binop)];
            if (binop_precedence < rhs_precedence) {
                rhs_result = parse_rhs(lexer, rhs_result.unwrap(), binop_precedence);
                if (rhs_result.is_error()) {
                    return rhs_result;
                }
            }
        }

        lhs_expression = ast::BinaryOperatorExpression::alloc(std::move(lhs_expression),
                                                              rhs_result.unwrap(), binop);
    }
}

util::Result<ast::Expression> parse_whole_expression(Lexer& lexer) {
    auto lhs = parse_lhs(lexer);
    if (lhs.is_error()) {
        return lhs;
    }

    return parse_rhs(lexer, lhs.unwrap(), Precendence::Unknown);
}

}  // namespace

util::Result<ast::Type> Parser::parse_type(Lexer& lexer) { return parse_whole_type(lexer); }

util::Result<ast::Expression> Parser::parse_expression(Lexer& lexer) {
    return parse_whole_expression(lexer);
}

}  // namespace cirrus::lang
