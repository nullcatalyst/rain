#include "gtest/gtest.h"
#include "rain/spec/helpers.hpp"

TEST(ParseExpression, integer) { EXPECT_TRUE(check_success(compile_expression("42"))); }
TEST(ParseExpression, floating_point) { EXPECT_TRUE(check_success(compile_expression("123.456"))); }
TEST(ParseExpression, boolean_true) { EXPECT_TRUE(check_success(compile_expression("true"))); }
TEST(ParseExpression, boolean_false) { EXPECT_TRUE(check_success(compile_expression("false"))); }
TEST(ParseExpression, member) { EXPECT_TRUE(check_success(compile_expression("my.prop"))); }
TEST(ParseExpression, ctor_empty) { EXPECT_TRUE(check_success(compile_expression("A {}"))); }
TEST(ParseExpression, ctor_single_field) {
    EXPECT_TRUE(check_success(compile_expression("A { a: 1 }")));
}
TEST(ParseExpression, ctor_single_field_comma) {
    EXPECT_TRUE(check_success(compile_expression("A { a: 1, }")));
}
TEST(ParseExpression, ctor_multi_field) {
    EXPECT_TRUE(check_success(compile_expression("A { a: 1, b: 2, c: 3 }")));
}
