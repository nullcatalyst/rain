#include "rain/spec/util.hpp"

TEST(Global, declare_primitive) {
    const std::string_view code = R"(
let global_variable = 42
)";

    EXPECT_COMPILE_SUCCESS(code);
}

TEST(Global, declare_simd) {
    const std::string_view code = R"(
let global_variable = f32x4 {
    x: 1.0,
    y: 2.0,
    z: 3.0,
    w: 4.0,
}
)";

    EXPECT_COMPILE_SUCCESS(code);
}

TEST(Global, declare_array) {
    const std::string_view code = R"(
let global_variable = [3]i32 { 1, 2, 3 }
)";

    EXPECT_COMPILE_SUCCESS(code);
}

TEST(Global, declare_struct) {
    const std::string_view code = R"(
struct Vec3 {
    x: f32,
    y: f32,
    z: f32,
}

let v3_zero = Vec3 {
    x: 1.0,
    y: 2.0,
    z: 3.0,
}
)";

    EXPECT_COMPILE_SUCCESS(code);
}

TEST(Global, assign_primitive) {
    const std::string_view code = R"(
let global_variable = 42

export fn get_variable() -> i32 {
    global_variable
}

export fn set_variable(new_value: i32) {
    global_variable = new_value
}
)";

    EXPECT_COMPILE_SUCCESS(code);
}
