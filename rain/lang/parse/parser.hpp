#pragma once

#include "rain/ast/expr/expression.hpp"
#include "rain/ast/type/type.hpp"
#include "rain/lang/lexer.hpp"
#include "rain/lang/module.hpp"
#include "rain/util/result.hpp"

namespace rain::lang {

class Parser {
  public:
    Parser()  = default;
    ~Parser() = default;

    [[nodiscard]] util::Result<ast::TypePtr>       parse_type(Lexer& lexer);
    [[nodiscard]] util::Result<ast::ExpressionPtr> parse_expression(Lexer& lexer);

    [[nodiscard]] util::Result<Module> parse(Lexer& lexer);
};

}  // namespace rain::lang
