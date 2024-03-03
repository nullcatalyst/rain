#include <string_view>

#include "gtest/gtest.h"
#include "rain/lang/lexer.hpp"
#include "rain/lang/parser.hpp"

namespace {

rain::util::Result<rain::lang::ParsedModule> test_compile(const std::string_view code) {
    rain::lang::Lexer  lexer(code);
    rain::lang::Parser parser;
    return parser.parse(lexer);
}

void test_compile_success(const std::string_view code) {
    rain::lang::Lexer  lexer(code);
    rain::lang::Parser parser;
    auto               result = parser.parse(lexer);
    ASSERT_NOT_ERROR(result);
}

void test_compile_error(const std::string_view code) {
    rain::lang::Lexer  lexer(code);
    rain::lang::Parser parser;
    auto               result = parser.parse(lexer);
    ASSERT_ERROR(result);
}

}  // namespace

TEST(ParseFunction, Empty) {
    test_compile_success(R"(
fn empty_function() {}
)");
}

TEST(ParseFunction, ExplicitReturn) {
    test_compile_success(R"(
fn explicit_return() -> i32 {
    return 0;
}
)");
}

TEST(ParseFunction, ImplicitReturn) {
    test_compile_success(R"(
fn implicit_return() -> i32 {
    0
}
)");
}

TEST(ParseFunction_error, MissingClosingBracket) {
    test_compile_error(R"(
fn missing_closing_bracket() -> i32 {
    0
)");
}

TEST(ParseFunction_error, MissingSemicolon) {
    test_compile_error(R"(
fn missing_semicolon() -> i32 {
    0
    0
}
)");
}
