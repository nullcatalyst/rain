#pragma once

#include <memory>

#include "rain/lang/lex/lexer.hpp"
#include "rain/util/assert.hpp"
#include "rain/util/result.hpp"

// Expressions
#include "rain/lang/ast/expr/block.hpp"
#include "rain/lang/ast/expr/boolean.hpp"
#include "rain/lang/ast/expr/call.hpp"
#include "rain/lang/ast/expr/compile_time.hpp"
#include "rain/lang/ast/expr/expression.hpp"
#include "rain/lang/ast/expr/extern.hpp"
#include "rain/lang/ast/expr/float.hpp"
#include "rain/lang/ast/expr/function.hpp"
#include "rain/lang/ast/expr/function_declaration.hpp"
#include "rain/lang/ast/expr/identifier.hpp"
#include "rain/lang/ast/expr/if.hpp"
#include "rain/lang/ast/expr/integer.hpp"
#include "rain/lang/ast/expr/let.hpp"
#include "rain/lang/ast/expr/member.hpp"
#include "rain/lang/ast/expr/module.hpp"
#include "rain/lang/ast/expr/parenthesis.hpp"
#include "rain/lang/ast/expr/struct_literal.hpp"
#include "rain/lang/ast/expr/type.hpp"
#include "rain/lang/ast/expr/while.hpp"

// Types
#include "rain/lang/ast/type/interface.hpp"
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
util::Result<std::unique_ptr<ast::FloatExpression>>      parse_float(lex::Lexer& lexer,
                                                                     ast::Scope& scope);
util::Result<std::unique_ptr<ast::IdentifierExpression>> parse_identifier(lex::Lexer& lexer,
                                                                          ast::Scope& scope);
util::Result<std::unique_ptr<ast::Expression>>           parse_struct_literal(lex::Lexer& lexer,
                                                                              ast::Scope& scope);
util::Result<std::unique_ptr<ast::LetExpression>> parse_let(lex::Lexer& lexer, ast::Scope& scope);
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
util::Result<std::unique_ptr<ast::FunctionDeclarationExpression>> parse_function_declaration(
    lex::Lexer& lexer, ast::Scope& scope);
util::Result<std::unique_ptr<ast::FunctionExpression>> parse_function(lex::Lexer& lexer,
                                                                      ast::Scope& scope);
util::Result<std::unique_ptr<ast::IfExpression>>     parse_if(lex::Lexer& lexer, ast::Scope& scope);
util::Result<std::unique_ptr<ast::WhileExpression>>  parse_while(lex::Lexer& lexer,
                                                                 ast::Scope& scope);
util::Result<std::unique_ptr<ast::ExternExpression>> parse_extern(lex::Lexer& lexer,
                                                                  ast::Scope& scope);

// Types
util::Result<std::unique_ptr<ast::Type>> parse_any_type(lex::Lexer& lexer, ast::Scope& scope);

util::Result<std::unique_ptr<ast::ArrayType>>     parse_array_type(lex::Lexer& lexer,
                                                                   ast::Scope& scope);
util::Result<std::unique_ptr<ast::OptionalType>>  parse_optional_type(lex::Lexer& lexer,
                                                                      ast::Scope& scope);
util::Result<std::unique_ptr<ast::StructType>>    parse_struct_type(lex::Lexer& lexer,
                                                                    ast::Scope& scope);
util::Result<std::unique_ptr<ast::InterfaceType>> parse_interface_type(lex::Lexer& lexer,
                                                                       ast::Scope& scope);

}  // namespace rain::lang::parse
