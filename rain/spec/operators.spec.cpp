#include "rain/spec/util.hpp"

#define DO_PRINT 1
#define DO_OPTIMIZE 1

TEST(Operators, cast) {
    const std::string_view code = R"(
export fn i64_to_i32(value: i64) -> i32 {
    value as i32
}

export fn f32_to_i32(value: f32) -> i32 {
    value as i32
}

export fn f64_to_i32(value: f64) -> i32 {
    value as i32
}
)";

    EXPECT_COMPILE_SUCCESS(code);
}
