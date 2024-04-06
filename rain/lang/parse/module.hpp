#pragma once

#include <memory>

#include "rain/lang/ast/module.hpp"
#include "rain/lang/ast/scope/builtin.hpp"
#include "rain/lang/lex/lexer.hpp"
#include "rain/util/result.hpp"

namespace rain::lang::parse {

util::Result<std::unique_ptr<ast::Module>> parse_module(lex::Lexer&        lexer,
                                                        ast::BuiltinScope& builtin);

}  // namespace rain::lang::parse
