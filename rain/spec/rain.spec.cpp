
#include <string_view>

#include "gtest/gtest.h"

// This must be included after gtest.h because the util::Result class checks if gtest was included
// in order to add additional functionality.
#include "rain/lang/target/wasm/init.hpp"
#include "rain/lang/target/wasm/options.hpp"
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

struct Mat4 {
    x: f32x4,
    y: f32x4,
    z: f32x4,
    w: f32x4,
}

export fn Mat4.zero() -> Mat4 {
    Mat4 {
        x: f32x4.zero(),
        y: f32x4.zero(),
        z: f32x4.zero(),
        w: f32x4.zero(),
    }
}

fn f32x4.dot(self, other: f32x4) -> f32 {
    self.x * other.x + self.y * other.y + self.z * other.z + self.w * other.w
}

export fn test_f32x4_dot(a: f32x4, b: f32x4) -> f32 {
    a.dot(b)
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

export fn array() -> [4]i32 {
   [4]i32{1, 2, compile_time_fib(), 4}
}
)";

    rain::lang::wasm::initialize_llvm();

    auto module_result = rain::compile(code);
    ASSERT_TRUE(check_success(module_result));
    auto mod = std::move(module_result).value();

    mod.optimize();

    auto ir_result = mod.emit_ir();
    ASSERT_TRUE(check_success(ir_result));
    auto ir = std::move(ir_result).value();

    std::cout << ir << std::endl;
}
