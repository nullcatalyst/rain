#include "gtest/gtest.h"
#include "rain/spec/helpers.hpp"

TEST(ParseStruct, empty_struct) {
    EXPECT_TRUE(test_compile(R"(
struct Empty {}
)"));
}

TEST(ParseStruct, simple_struct) {
    EXPECT_TRUE(test_compile(R"(
struct Simple {
    a: i32;
    b: f32;
}
)"));
}

TEST(ParseStruct, struct_method) {
    EXPECT_TRUE(test_compile(R"(
struct A {
    a: i32;
}

fn A.zero() -> i32 {
    return 0;
}
)"));
}
