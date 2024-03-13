#pragma once

#include <memory>

#include "rain/lang/ast/expr/integer.hpp"
#include "rain/lang/lex/lexer.hpp"
#include "rain/util/result.hpp"

namespace rain::lang::parse {

util::Result<std::unique_ptr<ast::Expression>> parse_any_expression(lex::Lexer& lexer,
                                                                    Scope&      scope);

util::Result<std::unique_ptr<ast::Expression>> parse_module(lex::Lexer& lexer, BuiltinScope& scope);
util::Result<std::unique_ptr<ast::IntegerExpression>>  parse_integer(lex::Lexer& lexer,
                                                                     Scope&      scope);
util::Result<std::unique_ptr<ast::IntegerExpression>>  parse_block(lex::Lexer& lexer, Scope& scope);
util::Result<std::unique_ptr<ast::FunctionExpression>> parse_function(lex::Lexer& lexer,
                                                                      Scope&      scope);

}  // namespace rain::lang::parse
