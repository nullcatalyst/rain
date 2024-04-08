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

// TEST(Function, nested) {
//     const std::string_view code = R"(
// export fn double_four() -> i32 {
//     2 * four()

//     fn four() -> i32 {
//         4
//     }
// }
// )";

//     EXPECT_COMPILE_SUCCESS(code);
// }
