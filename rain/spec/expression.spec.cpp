#include "gtest/gtest.h"
#include "rain/spec/helpers.hpp"

TEST(ParseExpression, Integer) {
    EXPECT_TRUE(test_compile(R"(
fn run() {
    42;
}
)"));
}

// TEST(ParseExpression, Boolean) {
//     EXPECT_TRUE(test_compile(R"(
// fn run() {
//     true;
// }
// )"));

//     EXPECT_TRUE(test_compile(R"(
// fn run() {
//     false;
// }
// )"));
// }

TEST(ParseExpression, Member) {
    EXPECT_TRUE(test_compile(R"(
fn run() {
    100.sqrt();
}
)"));
}