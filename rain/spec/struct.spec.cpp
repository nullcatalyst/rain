#include "gtest/gtest.h"
#include "rain/spec/helpers.hpp"

TEST(ParseStruct, Empty) {
    EXPECT_TRUE(test_compile(R"(
struct Empty {}
)"));
}

TEST(ParseStruct, Simple) {
    EXPECT_TRUE(test_compile(R"(
struct Simple {
    a: i32;
    b: f32;
}
)"));
}

TEST(ParseStruct, Method) {
    EXPECT_TRUE(test_compile(R"(
struct A {
    a: i32;
}

fn A.zero() -> i32 {
    return 0;
}
)"));
}
