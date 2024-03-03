#pragma once

#include "rain/ast/expr/all.hpp"
#include "rain/ast/type/type.hpp"
#include "rain/lang/lexer.hpp"
#include "rain/util/result.hpp"

namespace rain::lang {

util::Result<ast::ExpressionPtr> parse_whole_expression(Lexer& lexer);
util::Result<ast::TypePtr>       parse_whole_type(Lexer& lexer);

util::Result<ast::ExpressionPtr> parse_standard_expression(Lexer& lexer);

util::Result<ast::ExpressionPtr>                       parse_atom(Lexer& lexer);
util::Result<std::unique_ptr<ast::ExportExpression>>   parse_export(Lexer& lexer);
util::Result<std::unique_ptr<ast::FunctionExpression>> parse_function(Lexer& lexer);
util::Result<std::unique_ptr<ast::BlockExpression>>    parse_block(Lexer& lexer);
util::Result<std::unique_ptr<ast::IfExpression>>       parse_if(Lexer& lexer);

}  // namespace rain::lang
