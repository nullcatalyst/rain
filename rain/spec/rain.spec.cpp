
#include <string_view>

#include "gtest/gtest.h"

// This must be included after gtest.h because the util::Result class checks if gtest was included
// in order to add additional functionality.
#include "rain/lang/target/wasm/init.hpp"
#include "rain/lang/target/wasm/options.hpp"
#include "rain/rain.hpp"

#define DO_OPTIMIZE true

#define RUN_TEST($code)                              \
    do {                                             \
        rain::lang::wasm::initialize_llvm();         \
                                                     \
        auto module_result = rain::compile($code);   \
        ASSERT_TRUE(check_success(module_result));   \
        auto mod = std::move(module_result).value(); \
                                                     \
        if (DO_OPTIMIZE) {                           \
            mod.optimize();                          \
        }                                            \
                                                     \
        auto ir_result = mod.emit_ir();              \
        ASSERT_TRUE(check_success(ir_result));       \
        auto ir = std::move(ir_result).value();      \
                                                     \
        std::cout << ir << std::endl;                \
    } while (false)

TEST(Lang, struct_declaration) {
    const std::string_view code = R"(
struct Vec2 {
    x: f32,
    y: f32,
}
)";

    RUN_TEST(code);
}

TEST(Lang, struct_out_of_order_declaration) {
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

    RUN_TEST(code);
}

TEST(Lang, function) {
    const std::string_view code = R"(
export fn four() -> i32 {
    4
}
)";

    RUN_TEST(code);
}

TEST(Lang, array) {
    const std::string_view code = R"(
export fn int_array() -> [4]i32 {
    [4]i32{ 1, 2, 3, 4 }
}
)";

    RUN_TEST(code);
}

TEST(Lang, array_of_struct) {
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

    RUN_TEST(code);
}

TEST(Lang, optional) {
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

    RUN_TEST(code);
}
/*
TEST(Lang, struct) {
    const std::string_view code = R"(
struct Vec2 {
    x: f32,
    y: f32,
}

export fn Vec2.new(x: f32, y: f32) -> Vec2 {
    Vec2{ x: 1.0, y: 2.0 }
}
)";

    RUN_TEST(code);
}

TEST(Lang, operator_overload) {
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

    RUN_TEST(code);
}
*/
