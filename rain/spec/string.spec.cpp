#include "rain/spec/util.hpp"

TEST(String, length) {
    const std::string_view code = R"(
export fn string_length() -> i32 {
    "Hello, world!".length()
}
)";

    EXPECT_COMPILE_SUCCESS(code);
}
