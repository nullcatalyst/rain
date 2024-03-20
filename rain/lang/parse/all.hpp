#pragma once

#include <memory>

#include "rain/lang/lex/lexer.hpp"
#include "rain/util/result.hpp"

// Expressions
#include "rain/lang/ast/expr/block.hpp"
#include "rain/lang/ast/expr/call.hpp"
#include "rain/lang/ast/expr/expression.hpp"
#include "rain/lang/ast/expr/function.hpp"
#include "rain/lang/ast/expr/identifier.hpp"
#include "rain/lang/ast/expr/integer.hpp"
#include "rain/lang/ast/expr/member.hpp"
#include "rain/lang/ast/expr/module.hpp"
#include "rain/lang/ast/expr/type.hpp"

// Types
#include "rain/lang/ast/type/struct.hpp"
#include "rain/lang/ast/type/type.hpp"
#include "rain/lang/ast/type/unresolved.hpp"

// Scopes
#include "rain/lang/ast/scope/builtin.hpp"
#include "rain/lang/ast/scope/scope.hpp"

namespace rain::lang::parse {

util::Result<std::unique_ptr<ast::Module>> parse_module(lex::Lexer&        lexer,
                                                        ast::BuiltinScope& scope);

// Expressions
util::Result<std::unique_ptr<ast::Expression>> parse_any_expression(lex::Lexer& lexer,
                                                                    ast::Scope& scope);

util::Result<std::unique_ptr<ast::IntegerExpression>>    parse_integer(lex::Lexer& lexer,
                                                                       ast::Scope& scope);
util::Result<std::unique_ptr<ast::IdentifierExpression>> parse_identifier(lex::Lexer& lexer,
                                                                          ast::Scope& scope);
util::Result<std::unique_ptr<ast::Expression>> parse_atom(lex::Lexer& lexer, ast::Scope& scope);
util::Result<std::unique_ptr<ast::Expression>> parse_rhs(
    lex::Lexer& lexer, ast::Scope& scope, std::unique_ptr<ast::Expression> lhs_expression);
util::Result<std::unique_ptr<ast::MemberExpression>> parse_member(
    lex::Lexer& lexer, ast::Scope& scope, std::unique_ptr<ast::Expression> owner);
util::Result<std::unique_ptr<ast::CallExpression>> parse_call(
    lex::Lexer& lexer, ast::Scope& scope, std::unique_ptr<ast::Expression> callee);
util::Result<std::unique_ptr<ast::BlockExpression>>    parse_block(lex::Lexer& lexer,
                                                                   ast::Scope& scope);
util::Result<std::unique_ptr<ast::FunctionExpression>> parse_function(lex::Lexer& lexer,
                                                                      ast::Scope& scope);

// Types
util::Result<std::unique_ptr<ast::Type>> parse_any_type(lex::Lexer& lexer, ast::Scope& scope);

util::Result<std::unique_ptr<ast::StructType>> parse_struct(lex::Lexer& lexer, ast::Scope& scope);

}  // namespace rain::lang::parse
