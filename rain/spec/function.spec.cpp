#include "rain/spec/util.hpp"

TEST(Function, definition) {
    const std::string_view code = R"(
export fn four() -> i32 {
    4
}
)";

    EXPECT_COMPILE_SUCCESS(code);
}

TEST(Function, calling_another) {
    const std::string_view code = R"(
export fn four() -> i32 {
    4
}

export fn double_four() -> i32 {
    2 * four()
}
)";

    EXPECT_COMPILE_SUCCESS(code);
}

TEST(Function, out_of_order_definition) {
    const std::string_view code = R"(
export fn double_four() -> i32 {
    2 * four()
}

export fn four() -> i32 {
    4
}
)";

    EXPECT_COMPILE_SUCCESS(code);
}

TEST(Function, nested) {
    const std::string_view code = R"(
export fn double_four() -> i32 {
    fn four() -> i32 {
        4
    }

    2 * four()
}
)";

    EXPECT_COMPILE_SUCCESS(code);
}

TEST(Function, call_static_method) {
    const std::string_view code = R"(
fn f32x4.zero() -> f32x4 {
    f32x4 {
        x: 0.0,
        y: 0.0,
        z: 0.0,
        w: 0.0,
    }
}

export fn call_static_method() -> f32 {
    f32x4.zero().y
}
)";

    EXPECT_COMPILE_SUCCESS(code);
}

TEST(Function, if_else) {
    const std::string_view code = R"(
export fn conditional() -> i32 {
    if true {
        2 * 4
    } else {
        2 * 5
    }
}
)";

    EXPECT_COMPILE_SUCCESS(code);
}

TEST(Function, if_else_if) {
    const std::string_view code = R"(
export fn conditional() -> i32 {
    if false {
        2 * 4
    } else if true {
        2 * 5
    } else {
        2 * 6
    }
}
)";

    EXPECT_COMPILE_SUCCESS(code);
}
