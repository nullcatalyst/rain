#include "gtest/gtest.h"
#include "rain/spec/helpers.hpp"

TEST(ParseStruct, empty_struct) { EXPECT_TRUE(check_success(compile("struct Empty {}"))); }

TEST(ParseStruct, simple_struct) {
    EXPECT_TRUE(check_success(compile("struct Simple { a: i32, b: f32 }")));
}

TEST(ParseStruct, struct_method) {
    EXPECT_TRUE(check_success(compile("struct A { a: i32 }\nfn A.zero() -> i32 { 0 }")));
}
