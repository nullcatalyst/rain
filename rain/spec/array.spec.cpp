#include "rain/spec/util.hpp"

TEST(Array, literal) {
    const std::string_view code = R"(
export fn int_array() -> [4]i32 {
    [4]i32{ 1, 2, 3, 4 }
}
)";

    EXPECT_COMPILE_SUCCESS(code);
}

TEST(Array, array_of_struct) {
    const std::string_view code = R"(
struct Vec2 {
    x: f32,
    y: f32,
}

export fn array_of_structs() -> [2]Vec2 {
    [2]Vec2{
        Vec2{ x: 1.0, y: 2.0 },
        Vec2{ x: 3.0, y: 4.0 },
    }
}
)";

    EXPECT_COMPILE_SUCCESS(code);
}
