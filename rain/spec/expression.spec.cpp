#include "gtest/gtest.h"
#include "rain/spec/helpers.hpp"

TEST(ParseExpression, integer) { EXPECT_TRUE(test_compile("42")); }
TEST(ParseExpression, floating_point) { EXPECT_TRUE(test_compile("123.456")); }
TEST(ParseExpression, boolean_true) { EXPECT_TRUE(test_compile("true")); }
TEST(ParseExpression, boolean_false) { EXPECT_TRUE(test_compile("false")); }
TEST(ParseExpression, member) { EXPECT_TRUE(test_compile("my.prop")); }
TEST(ParseExpression, ctor_empty) { EXPECT_TRUE(test_compile("A {}")); }
TEST(ParseExpression, ctor_single_field) { EXPECT_TRUE(test_compile("A { a: 1 }")); }
TEST(ParseExpression, ctor_single_field_comma) { EXPECT_TRUE(test_compile("A { a: 1, }")); }
TEST(ParseExpression, ctor_multi_field) { EXPECT_TRUE(test_compile("A { a: 1, b: 2, c: 3 }")); }
