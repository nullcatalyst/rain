#include "rain/spec/util.hpp"

TEST(Integration, fib_iteration) {
    const std::string_view code = R"(
export fn fib_iteration(n: i32) -> i32 {
    let a = 0
    let b = 1

    let n = n
    while n > 0 {
        let tmp = a
        a = b
        b = tmp + b
        n = n - 1
    }

    a
}
)";

    EXPECT_COMPILE_SUCCESS(code);
}

TEST(Integration, mat4) {
    const std::string_view code = R"(
fn f32x4.new(x: f32, y: f32, z: f32, w: f32) -> f32x4 {
    f32x4 { x: x, y: y, z: z, w: w }
}

// Structs can be declared in a fairly standard way:
struct Mat4 {
    x: f32x4,
    y: f32x4,
    z: f32x4,
    w: f32x4,
}

fn Mat4.identity() -> Mat4 {
    Mat4 {
        x: f32x4.new(1.0, 0.0, 0.0, 0.0),
        y: f32x4.new(0.0, 1.0, 0.0, 0.0),
        z: f32x4.new(0.0, 0.0, 1.0, 0.0),
        w: f32x4.new(0.0, 0.0, 0.0, 1.0),
    }
}
)";

    EXPECT_COMPILE_SUCCESS(code);
}
