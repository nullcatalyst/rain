#include "rain/spec/util.hpp"

TEST(Slice, slice_of_ints) {
    const std::string_view code = R"(
export fn slice_of_ints() -> []i32 {
    []i32{ 1, 2, 3, 4 }
}
)";

    EXPECT_COMPILE_SUCCESS(code);
}

TEST(Slice, slice_of_structs) {
    const std::string_view code = R"(
struct Vec2 {
    x: f32,
    y: f32,
}

export fn slice_of_structs() -> [2]Vec2 {
    [2]Vec2{
        Vec2{ x: 1.0, y: 2.0 },
        Vec2{ x: 3.0, y: 4.0 },
    }
}
)";

    EXPECT_COMPILE_SUCCESS(code);
}

TEST(Slice, indexing) {
    const std::string_view code = R"(
export fn slice_indexing(index: i32) -> i32 {
    let slice = []i32{ 1, 2, 3, 4 }
    slice[index]
}
)";

    EXPECT_COMPILE_SUCCESS(code);
}

TEST(Slice, length) {
    const std::string_view code = R"(
export fn slice_length() -> i32 {
    []i32{ 1, 2, 3, 4 }.length()
}
)";

    EXPECT_COMPILE_SUCCESS(code);
}

TEST(Slice, index_assignment) {
    const std::string_view code = R"(
export fn slice_indexed_assignment() -> i32 {
    let slice = []i32{ 1, 2, 3, 4 }
    slice[2] = 42
    slice[2]
}
)";

    EXPECT_COMPILE_SUCCESS(code);
}
