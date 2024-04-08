#include "rain/spec/util.hpp"

TEST(Struct, struct_definition) {
    const std::string_view code = R"(
struct Vec2 {
    x: f32,
    y: f32,
}
)";

    EXPECT_COMPILE_SUCCESS(code);
}

TEST(Struct, out_of_order_definition) {
    const std::string_view code = R"(
struct Entity {
    position: Vec2,
    velocity: Vec2,
}

struct Vec2 {
    x: f32,
    y: f32,
}
)";

    EXPECT_COMPILE_SUCCESS(code);
}

TEST(Struct, method) {
    const std::string_view code = R"(
struct Vec2 {
    x: f32,
    y: f32,
}

fn Vec2.new(x: f32, y: f32) -> Vec2 {
    Vec2 {
        x: x,
        y: y,
    }
}
)";

    EXPECT_COMPILE_SUCCESS(code);
}

TEST(Struct, operator_overload) {
    const std::string_view code = R"(
struct Vec2 {
    x: f32,
    y: f32,
}

fn Vec2.__add__(self, other: Vec2) -> Vec2 {
    Vec2{ x: self.x + other.x, y: self.y + other.y }
}

export fn add_vecs() -> Vec2 {
    Vec2{ x: 1.0, y: 2.0 } + Vec2{ x: 3.0, y: 4.0 }
}
)";

    EXPECT_COMPILE_SUCCESS(code);
}
