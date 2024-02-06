#pragma once

#include "cirrus/ast/expr/expression.hpp"
#include "cirrus/ast/type/type.hpp"
#include "cirrus/lang/lexer.hpp"
#include "cirrus/lang/module.hpp"
#include "cirrus/util/result.hpp"

namespace cirrus::lang {

class Parser {
  public:
    Parser()  = default;
    ~Parser() = default;

    [[nodiscard]] util::Result<ast::Type>       parse_type(Lexer& lexer);
    [[nodiscard]] util::Result<ast::Expression> parse_expression(Lexer& lexer);

    [[nodiscard]] util::Result<ParsedModule> parse(Lexer& lexer);
};

}  // namespace cirrus::lang
