#include "rain/spec/util.hpp"

TEST(Reference, argument_type) {
    const std::string_view code = R"(
export fn pass_ref(value: &i32) -> i32 {
    value
}
)";

    EXPECT_COMPILE_SUCCESS(code);
}
