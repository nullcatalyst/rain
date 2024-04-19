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

TEST(Array, indexing) {
    const std::string_view code = R"(
export fn array_indexing(index: i32) -> i32 {
    let array = [4]i32{ 1, 2, 3, 4 }
    array[index]
}
)";

    EXPECT_COMPILE_SUCCESS(code);
}

TEST(Array, length) {
    const std::string_view code = R"(
export fn array_length() -> i32 {
    [4]i32{ 1, 2, 3, 4 }.length()
}
)";

    EXPECT_COMPILE_SUCCESS(code);
}

TEST(Array, assignment) {
    const std::string_view code = R"(
export fn array_assignment() -> i32 {
    let array = [4]i32{ 1, 2, 3, 4 }
    array[2] = 42
    array[2]
}
)";

    EXPECT_COMPILE_SUCCESS(code);
}
