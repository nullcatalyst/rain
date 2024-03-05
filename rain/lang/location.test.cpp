#include "gtest/gtest.h"
#include "rain/ast/expr/all.hpp"
#include "rain/ast/type/all.hpp"
#include "rain/lang/lexer.hpp"
#include "rain/lang/parser.hpp"

using namespace rain;
using namespace rain::lang;

namespace {

rain::util::Result<rain::ast::TypePtr> compile_type(const std::string_view code) {
    rain::lang::Lexer  lexer(code);
    rain::lang::Parser parser;
    return parser.parse_type(lexer);
}

rain::util::Result<rain::ast::ExpressionPtr> compile_expression(const std::string_view code) {
    rain::lang::Lexer  lexer(code);
    rain::lang::Parser parser;
    return parser.parse_expression(lexer);
}

template <typename T>
testing::AssertionResult check_result(const rain::util::Result<T>& result) {
    if (result.has_value()) {
        return testing::AssertionSuccess();
    }
    return testing::AssertionFailure() << result.error()->message();
}

}  // namespace

TEST(ExpressionLocation, parse_struct) {
    const std::string_view code = R"(
struct Point {
    x: i32;
    y: i32;
}
)";

    auto result = compile_type(code);
    EXPECT_TRUE(check_result(result));
    const auto type = std::move(result).value();

    const auto location = type->location();
    EXPECT_EQ(location.line(), 2);
    EXPECT_EQ(location.column(), 1);
    EXPECT_EQ(location.substr(), code);
}

TEST(ExpressionLocation, parse_integer) {
    const std::string_view code = "100";

    auto result = compile_expression(code);
    EXPECT_TRUE(check_result(result));
    const auto expr = std::move(result).value();

    const auto location = expr->location();
    EXPECT_EQ(location.line(), 1);
    EXPECT_EQ(location.column(), 1);
    EXPECT_EQ(location.substr(), code);
}

TEST(ExpressionLocation, parse_float) {
    const std::string_view code = "4.2";

    auto result = compile_expression(code);
    EXPECT_TRUE(check_result(result));
    const auto expr = std::move(result).value();

    const auto location = expr->location();
    EXPECT_EQ(location.line(), 1);
    EXPECT_EQ(location.column(), 1);
    EXPECT_EQ(location.substr(), code);
}

TEST(ExpressionLocation, parse_identifier) {
    const std::string_view code = "iden";

    auto result = compile_expression(code);
    EXPECT_TRUE(check_result(result));
    const auto expr = std::move(result).value();

    const auto location = expr->location();
    EXPECT_EQ(location.line(), 1);
    EXPECT_EQ(location.column(), 1);
    EXPECT_EQ(location.substr(), code);
}

TEST(ExpressionLocation, parse_member) {
    const std::string_view code = "my.prop";

    auto result = compile_expression(code);
    EXPECT_TRUE(check_result(result));
    const auto expr = std::move(result).value();

    const auto location = expr->location();
    EXPECT_EQ(location.line(), 1);
    EXPECT_EQ(location.column(), 1);
    EXPECT_EQ(location.substr(), code);
}

TEST(ExpressionLocation, parse_binary_operator) {
    const std::string_view code = "1 + 1";

    auto result = compile_expression(code);
    EXPECT_TRUE(check_result(result));
    const auto expr = std::move(result).value();

    const auto location = expr->location();
    EXPECT_EQ(location.line(), 1);
    EXPECT_EQ(location.column(), 1);
    EXPECT_EQ(location.substr(), code);
}

TEST(ExpressionLocation, parse_exec) {
    const std::string_view code = "#42";

    auto result = compile_expression(code);
    EXPECT_TRUE(check_result(result));
    const auto expr = std::move(result).value();

    const auto location = expr->location();
    EXPECT_EQ(location.line(), 1);
    EXPECT_EQ(location.column(), 1);
    EXPECT_EQ(location.substr(), code);
}

TEST(ExpressionLocation, parse_call) {
    const std::string_view code = "run()";

    auto result = compile_expression(code);
    EXPECT_TRUE(check_result(result));
    const auto expr = std::move(result).value();

    const auto location = expr->location();
    EXPECT_EQ(location.line(), 1);
    EXPECT_EQ(location.column(), 1);
    EXPECT_EQ(location.substr(), code);
}

TEST(ExpressionLocation, parse_function) {
    const std::string_view code = "fn ten() -> i32 { return 10; }";

    auto result = compile_expression(code);
    EXPECT_TRUE(check_result(result));
    const auto expr = std::move(result).value();

    const auto location = expr->location();
    EXPECT_EQ(location.line(), 1);
    EXPECT_EQ(location.column(), 1);
    EXPECT_EQ(location.substr(), code);
}

TEST(ExpressionLocation, parse_if) {
    const std::string_view code = "if true { 1 }";

    auto result = compile_expression(code);
    EXPECT_TRUE(check_result(result));
    const auto expr = std::move(result).value();

    const auto location = expr->location();
    EXPECT_EQ(location.line(), 1);
    EXPECT_EQ(location.column(), 1);
    EXPECT_EQ(location.substr(), code);
}

TEST(ExpressionLocation, parse_if_else) {
    const std::string_view code = "if true { 1 } else { 2 }";

    auto result = compile_expression(code);
    EXPECT_TRUE(check_result(result));
    const auto expr = std::move(result).value();

    const auto location = expr->location();
    EXPECT_EQ(location.line(), 1);
    EXPECT_EQ(location.column(), 1);
    EXPECT_EQ(location.substr(), code);
}
