#include "gtest/gtest.h"
#include "rain/spec/helpers.hpp"

TEST(ParseFunction, empty) {
    EXPECT_TRUE(test_compile(R"(
fn empty_function() {}
)"));
}

TEST(ParseFunction, explicit_return) {
    EXPECT_TRUE(test_compile(R"(
fn explicit_return() -> i32 {
    return 0;
}
)"));
}

TEST(ParseFunction, implicit_return) {
    EXPECT_TRUE(test_compile(R"(
fn implicit_return() -> i32 {
    0
}
)"));
}

TEST(ParseFunction_error, missing_closing_bracket) {
    EXPECT_FALSE(test_compile(R"(
fn missing_closing_bracket() -> i32 {
    0
)"));
}

TEST(ParseFunction_error, missing_semicolon_between_statements) {
    EXPECT_FALSE(test_compile(R"(
fn missing_semicolon() -> i32 {
    0
    0
}
)"));
}
