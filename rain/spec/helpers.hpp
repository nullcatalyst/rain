#pragma once

#include <string_view>

#include "gtest/gtest.h"
#include "rain/lang/lexer.hpp"
#include "rain/lang/parser.hpp"

namespace {

[[maybe_unused]] rain::util::Result<rain::lang::Module> compile(const std::string_view source) {
    rain::lang::Lexer  lexer = rain::lang::Lexer::from_memory("<memory>", source);
    rain::lang::Parser parser;
    return parser.parse(lexer);
}

[[maybe_unused]] rain::util::Result<rain::ast::TypePtr> compile_type(
    const std::string_view source) {
    rain::lang::Lexer  lexer = rain::lang::Lexer::from_memory("<memory>", source);
    rain::lang::Parser parser;
    return parser.parse_type(lexer);
}

[[maybe_unused]] rain::util::Result<rain::ast::ExpressionPtr> compile_expression(
    const std::string_view source) {
    rain::lang::Lexer  lexer = rain::lang::Lexer::from_memory("<memory>", source);
    rain::lang::Parser parser;
    return parser.parse_expression(lexer);
}

}  // namespace
