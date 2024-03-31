
#include <string_view>

#include "gtest/gtest.h"
//
#include "rain/lang/code/target/default.hpp"
#include "rain/lib/compile_time_functions.hpp"
#include "rain/rain.hpp"

TEST(Lang, struct) {
    const std::string_view code = R"(
fn f32x4.zero() -> f32x4 {
    f32x4 {
        x: 0.0,
        y: 0.0,
        z: 0.0,
        w: 0.0,
    }
}

fn f32x4.splat(value: f32) -> f32x4 {
    f32x4 {
        x: value,
        y: value,
        z: value,
        w: value,
    }
}

fn f32x4.new(x: f32, y: f32, z: f32, w: f32) -> f32x4 {
    f32x4 {
        x: x,
        y: y,
        z: z,
        w: w,
    }
}

export fn test_f32x4() -> f32 {
    f32x4 {
        x: 1.0,
        y: 2.0,
        z: 3.0,
        w: 4.0,
    }.z
}


// The last expression in a function is implicitly returned.
fn four() -> i32 {
    4
}

// Methods can be added to any type, at any time.
// But are only available in the scope they are defined.
fn i32.double(self) -> i32 {
    2 * self
}

// Example use of custom method on a primitive type.
fn double_double() -> i32 {
    4.double().double()
}

// Only exported functions are exported from WASM so they can be called from JavaScript.
// This is useful to optimize away unused code.
fn fib(n: i32) -> i32 {
    if n < 2 {
        1
    } else {
        fib(n - 1) + fib(n - 2)
    }
}

// Putting a # before any expression causes the expression
// to be evaluated at compile time.
//
// The safety rails of this feature are pretty minimal at the moment,
// so please be careful with it (no infinite loops, etc).
export fn compile_time_fib() -> i32 {
    #fib(8)
}

fn f32.sqrt(self) -> f32 {
    extern("js", "math", "sqrt") fn js_math_sqrt(value: f32) -> f32
    js_math_sqrt(self)
}

fn f32.cos(self) -> f32 {
    extern("js", "math", "cos") fn js_math_cos(value: f32) -> f32
    js_math_cos(self)
}

fn f32.sin(self) -> f32 {
    extern("js", "math", "sin") fn js_math_sin(value: f32) -> f32
    js_math_sin(self)
}

export fn root_2() -> f32 {
    #2.0.sqrt()
}
)";

    rain::lang::code::initialize_llvm();
    rain::load_external_functions_into_llvm_interpreter();

    auto module_result = rain::compile(code, rain::add_external_functions_to_module);
    ASSERT_TRUE(check_success(module_result));
    auto mod = std::move(module_result).value();

    mod.optimize();

    auto ir_result = mod.emit_ir();
    ASSERT_TRUE(check_success(ir_result));
    auto ir = std::move(ir_result).value();

    std::cout << ir << std::endl;
}
