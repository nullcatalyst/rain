#include "rain/spec/util.hpp"

TEST(Optional, return_type) {
    const std::string_view code = R"(
export fn no_int() -> ?i32 {
    null
}
export fn yes_int() -> ?i32 {
    42
}
export fn pass_int(value: ?i32) -> ?i32 {
    value
}
)";

    EXPECT_COMPILE_SUCCESS(code);
}
