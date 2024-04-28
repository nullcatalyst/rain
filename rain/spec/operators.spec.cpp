#include "rain/spec/util.hpp"

#define DO_PRINT 1
#define DO_OPTIMIZE 0

TEST(Operator, negative_literal) {
    const std::string_view code = R"(
export fn neg_i32() -> i32 {
    -1
}

export fn neg_i64() -> i64 {
    -(2 as i64)
}

export fn neg_f32() -> f32 {
    -3.0
}

export fn neg_f64() -> f64 {
    -(4.0 as f64)
}
)";

    EXPECT_COMPILE_SUCCESS(code);
}

TEST(Operator, negative) {
    const std::string_view code = R"(
export fn neg_i32(value: i32) -> i32 {
    -value
}

export fn neg_i64(value: i64) -> i64 {
    -value
}

export fn neg_f32(value: f32) -> f32 {
    -value
}

export fn neg_f64(value: f64) -> f64 {
    -value
}
)";

    EXPECT_COMPILE_SUCCESS(code);
}

TEST(Operator, precedence) {
    const std::string_view code = R"(
export fn a_plus_b_mul_c(a: f32, b: f32, c: f32) -> f32 {
    a + b * c
}

export fn a_mul_b_plus_c(a: f32, b: f32, c: f32) -> f32 {
    a * b + c
}
)";

    EXPECT_COMPILE_SUCCESS(code);
}

TEST(Cast, as_i32) {
    const std::string_view code = R"(
export fn i64_to_i32(value: i64) -> i32 {
    value as i32
}

export fn f32_to_i32(value: f32) -> i32 {
    value as i32
}

export fn f64_to_i32(value: f64) -> i32 {
    value as i32
}
)";

    EXPECT_COMPILE_SUCCESS(code);
}

TEST(Cast, as_i64) {
    const std::string_view code = R"(
export fn i32_to_i64(value: i32) -> i64 {
    value as i64
}

export fn f32_to_i64(value: f32) -> i64 {
    value as i64
}

export fn f64_to_i64(value: f64) -> i64 {
    value as i64
}
)";

    EXPECT_COMPILE_SUCCESS(code);
}

TEST(Cast, as_f32) {
    const std::string_view code = R"(
export fn i32_to_f32(value: i32) -> f32 {
    value as f32
}

export fn i64_to_f32(value: i64) -> f32 {
    value as f32
}

export fn f64_to_f32(value: f64) -> f32 {
    value as f32
}
)";

    EXPECT_COMPILE_SUCCESS(code);
}

TEST(Cast, as_f64) {
    const std::string_view code = R"(
export fn i32_to_f64(value: i32) -> f64 {
    value as f64
}

export fn i64_to_f64(value: i64) -> f64 {
    value as f64
}

export fn f32_to_f64(value: f32) -> f64 {
    value as f64
}
)";

    EXPECT_COMPILE_SUCCESS(code);
}
