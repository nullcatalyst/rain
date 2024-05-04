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

TEST(Optional, check_has_value) {
    const std::string_view code = R"(
export fn has_value(value: ?i32) -> bool {
    value?
}
)";

    EXPECT_COMPILE_SUCCESS(code);
}

TEST(Optional, unwrap) {
    const std::string_view code = R"(
export fn value_or(optional_value: ?i32, default_value: i32) -> i32 {
    if optional_value? {
        // The value is guaranteed to be present, so here it now has the i32 type, instead of ?i32.
        optional_value
    } else {
        default_value
    }
}
)";

    EXPECT_COMPILE_SUCCESS(code);
}
