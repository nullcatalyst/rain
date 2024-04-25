#include "absl/strings/str_cat.h"
#include "rain/lang/ast/expr/binary_operator.hpp"
#include "rain/lang/ast/expr/block.hpp"
#include "rain/lang/ast/expr/boolean.hpp"
#include "rain/lang/ast/expr/call.hpp"
#include "rain/lang/ast/expr/compile_time.hpp"
#include "rain/lang/ast/expr/expression.hpp"
#include "rain/lang/ast/expr/extern.hpp"
#include "rain/lang/ast/expr/float.hpp"
#include "rain/lang/ast/expr/function.hpp"
#include "rain/lang/ast/expr/identifier.hpp"
#include "rain/lang/ast/expr/if.hpp"
#include "rain/lang/ast/expr/integer.hpp"
#include "rain/lang/ast/expr/let.hpp"
#include "rain/lang/ast/expr/member.hpp"
#include "rain/lang/ast/expr/null.hpp"
#include "rain/lang/ast/expr/parenthesis.hpp"
#include "rain/lang/ast/expr/unary_operator.hpp"
#include "rain/lang/ast/expr/while.hpp"
#include "rain/lang/ast/scope/scope.hpp"
#include "rain/lang/err/syntax.hpp"
#include "rain/lang/lex/lexer.hpp"
#include "rain/util/result.hpp"

namespace rain::lang::parse {

util::Result<std::unique_ptr<ast::Expression>> parse_any_expression(lex::Lexer& lexer,
                                                                    ast::Scope& scope);

util::Result<std::unique_ptr<ast::IntegerExpression>>    parse_integer(lex::Lexer& lexer,
                                                                       ast::Scope& scope);
util::Result<std::unique_ptr<ast::FloatExpression>>      parse_float(lex::Lexer& lexer,
                                                                     ast::Scope& scope);
util::Result<std::unique_ptr<ast::IdentifierExpression>> parse_identifier(lex::Lexer& lexer,
                                                                          ast::Scope& scope);
util::Result<std::unique_ptr<ast::Expression>>           parse_struct_literal(lex::Lexer& lexer,
                                                                              ast::Scope& scope);
util::Result<std::unique_ptr<ast::LetExpression>> parse_let(lex::Lexer& lexer, ast::Scope& scope,
                                                            bool global);
util::Result<std::unique_ptr<ast::Expression>>    parse_atom(lex::Lexer& lexer, ast::Scope& scope);
util::Result<std::unique_ptr<ast::Expression>>    parse_unary_operator(lex::Lexer& lexer,
                                                                       ast::Scope& scope);
util::Result<std::unique_ptr<ast::Expression>>    parse_rhs(
       lex::Lexer& lexer, ast::Scope& scope, std::unique_ptr<ast::Expression> lhs_expression);
util::Result<std::unique_ptr<ast::MemberExpression>> parse_member(
    lex::Lexer& lexer, ast::Scope& scope, std::unique_ptr<ast::Expression> owner);
util::Result<std::unique_ptr<ast::CallExpression>> parse_call(
    lex::Lexer& lexer, ast::Scope& scope, std::unique_ptr<ast::Expression> callee);
util::Result<std::unique_ptr<ast::CompileTimeExpression>> parse_compile_time(lex::Lexer& lexer,
                                                                             ast::Scope& scope);
util::Result<std::unique_ptr<ast::ParenthesisExpression>> parse_parenthesis(lex::Lexer& lexer,
                                                                            ast::Scope& scope);
util::Result<std::unique_ptr<ast::BlockExpression>>       parse_block(lex::Lexer& lexer,
                                                                      ast::Scope& scope);
util::Result<std::unique_ptr<ast::FunctionExpression>>    parse_function(
       lex::Lexer& lexer, ast::Scope& scope, bool allow_callee_type, bool allow_self_argument,
       absl::Nullable<ast::Type*> default_callee_type);
util::Result<std::unique_ptr<ast::IfExpression>>     parse_if(lex::Lexer& lexer, ast::Scope& scope);
util::Result<std::unique_ptr<ast::WhileExpression>>  parse_while(lex::Lexer& lexer,
                                                                 ast::Scope& scope);
util::Result<std::unique_ptr<ast::ExternExpression>> parse_extern(lex::Lexer& lexer,
                                                                  ast::Scope& scope);

namespace {

util::Result<std::unique_ptr<ast::Expression>> parse_standalone_atom(lex::Lexer& lexer,
                                                                     ast::Scope& scope) {
    const auto token = lexer.peek();
    switch (token.kind) {
        case lex::TokenKind::False: {
            lexer.next();  // Consume the `false` token
            return std::make_unique<ast::BooleanExpression>(false, token.location);
        }

        case lex::TokenKind::True: {
            lexer.next();  // Consume the `true` token
            return std::make_unique<ast::BooleanExpression>(true, token.location);
        }

        case lex::TokenKind::Integer:
            return parse_integer(lexer, scope);

        case lex::TokenKind::Float:
            return parse_float(lexer, scope);

        case lex::TokenKind::Identifier:
            return parse_identifier(lexer, scope);

        case lex::TokenKind::Self: {
            lexer.next();  // Consume the `self` token
            return std::make_unique<ast::IdentifierExpression>(token.text(), token.location);
        }

        case lex::TokenKind::Null: {
            lexer.next();  // Consume the `null` token
            return std::make_unique<ast::NullExpression>(token.location);
        }

        case lex::TokenKind::Let:
            return parse_let(lexer, scope, false);

        case lex::TokenKind::LRoundBracket:
            return parse_parenthesis(lexer, scope);

        case lex::TokenKind::LCurlyBracket:
            return parse_block(lexer, scope);

        case lex::TokenKind::If:
            return parse_if(lexer, scope);

        case lex::TokenKind::While:
            return parse_while(lexer, scope);

        case lex::TokenKind::Fn:
            return parse_function(lexer, scope, true, true, nullptr);

        case lex::TokenKind::Hash:
            return parse_compile_time(lexer, scope);

        case lex::TokenKind::Extern:
            return parse_extern(lexer, scope);

        default:
            return parse_unary_operator(lexer, scope);
    }
}

}  // namespace

util::Result<std::unique_ptr<ast::Expression>> parse_atom(lex::Lexer& lexer, ast::Scope& scope) {
    std::unique_ptr<ast::Expression> expression;

    {
        const auto state       = lexer.save_state();
        auto       ctor_result = parse_struct_literal(lexer, scope);
        if (ctor_result.has_value()) {
            expression = std::move(ctor_result).value();
        } else {
            lexer.restore_state(state);

            auto result = parse_standalone_atom(lexer, scope);
            FORWARD_ERROR(result);
            expression = std::move(result).value();
        }
    }

    auto next_token = lexer.peek();
    while (next_token.kind == lex::TokenKind::Question ||
           next_token.kind == lex::TokenKind::Period ||
           next_token.kind == lex::TokenKind::LRoundBracket ||
           next_token.kind == lex::TokenKind::LSquareBracket) {
        if (next_token.kind == lex::TokenKind::Question) {
            lexer.next();  // Consume the `?` token

            expression = std::make_unique<ast::UnaryOperatorExpression>(
                std::move(expression), serial::UnaryOperatorKind::HasValue, next_token.location);
            next_token = lexer.peek();
            continue;
        }

        if (next_token.kind == lex::TokenKind::Period) {
            auto result = parse_member(lexer, scope, std::move(expression));
            FORWARD_ERROR(result);

            expression = std::move(result).value();
            next_token = lexer.peek();
            continue;
        }

        if (next_token.kind == lex::TokenKind::LRoundBracket) {
            auto result = parse_call(lexer, scope, std::move(expression));
            FORWARD_ERROR(result);

            expression = std::move(result).value();
            next_token = lexer.peek();
            continue;
        }

        if (next_token.kind == lex::TokenKind::LSquareBracket) {
            lexer.next();  // Consume the `[` token

            auto result = parse_any_expression(lexer, scope);
            FORWARD_ERROR(result);
            auto index_expression = std::move(result).value();

            if (const auto rsquare_token = lexer.next();
                rsquare_token.kind != lex::TokenKind::RSquareBracket) {
                return ERR_PTR(err::SyntaxError, index_expression->location().empty_string_after(),
                               "expected ']' after array index expression");
            }

            expression = std::make_unique<ast::BinaryOperatorExpression>(
                std::move(expression), std::move(index_expression),
                serial::BinaryOperatorKind::ArrayIndex);
            next_token = lexer.peek();
            continue;
        }
    }

    return expression;
}

}  // namespace rain::lang::parse
