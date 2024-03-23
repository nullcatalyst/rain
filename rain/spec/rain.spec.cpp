
#include <string_view>

#include "gtest/gtest.h"
//
#include "rain/lang/code/target/default.hpp"
#include "rain/rain.hpp"

TEST(Lang, struct) {
    const std::string_view code = R"(
fn i32.double(self) -> i32 {
    2 * self
}

fn double_double(x: i32) -> i32 {
    x.double().double()
}

fn fib(n: i32) -> i32 {
    if n < 2 {
        1
    } else {
        fib(n - 1) + fib(n - 2)
    }
}

export fn compile_time_fib_6() -> i32 {
    #fib(6)
}
)";

    rain::lang::code::initialize_llvm();

    auto module_result = rain::compile(code);
    ASSERT_TRUE(check_success(module_result));
    auto mod = std::move(module_result).value();

    mod.optimize();

    auto ir_result = mod.emit_ir();
    ASSERT_TRUE(check_success(ir_result));
    auto ir = std::move(ir_result).value();

    std::cout << ir << std::endl;
}
