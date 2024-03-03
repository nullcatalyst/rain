#include <string_view>

#include "gtest/gtest.h"
#include "rain/lang/lexer.hpp"
#include "rain/lang/parser.hpp"

namespace {

void test_compile(const std::string_view code) {
    rain::lang::Lexer  lexer(code);
    rain::lang::Parser parser;
    auto               result = parser.parse(lexer);
    ASSERT_NOT_ERROR(result);
}

}  // namespace

TEST(ParseStruct, Empty) {
    test_compile(R"(
struct Empty {}
)");
}

TEST(ParseStruct, Simple) {
    test_compile(R"(
struct Simple {
    a: i32;
    b: f32;
}
)");
}

TEST(ParseStruct, Method) {
    test_compile(R"(
struct A {
    a: i32;
}

fn A.zero() -> i32 {
    return 0;
}
)");
}
