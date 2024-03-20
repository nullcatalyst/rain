#include "rain/bin/common.hpp"
#include "rain/bin/compile_time_functions.hpp"
#include "rain/lang/code/target/default.hpp"

#define RAIN_INCLUDE_COMPILE
#define RAIN_INCLUDE_LINK
#define RAIN_INCLUDE_DECOMPILE
#include "rain/rain.hpp"

WASM_EXPORT("init")
void initialize() {
    rain::lang::code::target::initialize_llvm();

    load_external_functions_into_llvm();
}

WASM_EXPORT("compile")
void compile(const char* source_start, const char* source_end) {
    static std::string prev_result;
    prev_result.clear();

    // Compile the source code.
    auto compile_result = rain::compile(
        std::string_view{source_start, source_end},
        [](rain::code::Compiler& compiler) { add_external_functions_to_compiler(compiler); });
    if (!compile_result.has_value()) {
        rain::throw_error(compile_result.error()->message());
    }
    auto rain_mod = std::move(compile_result).value();

    // Optimize the module.
    rain_mod.optimize();

    // Get the LLVM IR.
    auto ir_result = rain_mod.emit_ir();
    if (!ir_result.has_value()) {
        rain::throw_error(ir_result.error()->message());
    }
    auto ir = std::move(ir_result).value();
    callback(rain::Action::CompileRain, ir.c_str(), ir.c_str() + ir.size());

    // Link the module into WebAssembly.
    auto link_result = rain::link(std::string_view{source_start, source_end});
    if (!link_result.has_value()) {
        rain::throw_error(link_result.error()->message());
    }
    auto wasm = std::move(link_result).value();
    callback(rain::Action::CompileLLVM, &*wasm->string().begin(), &*wasm->string().begin());

    // Decompile the WebAssembly into WAT.
    auto decompile_result =
        rain::decompile(std::span{reinterpret_cast<const uint8_t*>(source_start),
                                  reinterpret_cast<const uint8_t*>(source_end)});
    if (!decompile_result.has_value()) {
        rain::throw_error(decompile_result.error()->message());
    }
    auto wat = std::move(decompile_result).value();
    callback(rain::Action::DecompileWasm, &*wat->string().begin(), &*wat->string().end());
}

#if !defined(__wasm__)

int main(const int argc, const char* const argv[]) {
    const std::string_view source = R"(
fn f32.cos(self) -> f32 {
    return __builtin_cos(self as f64) as f32;
}

fn f32.sin(self) -> f32 {
    return __builtin_sin(self as f64) as f32;
}

export fn f32x4.new(x: f32, y: f32, z: f32, w: f32) -> f32x4 {
    return f32x4 { x: x, y: y, z: z, w: w };
}

export fn f32x4.splat(x: f32) -> f32x4 {
    return f32x4 { x: x, y: x, z: x, w: x };
}

export fn f32x4.zero() -> f32x4 {
    return f32x4.splat(0.0);
}

// export
struct f32x4x4 {
    x: f32x4,
    y: f32x4,
    z: f32x4,
    w: f32x4,
}

export fn f32x4x4.zero() -> f32x4x4 {
    return f32x4x4 {
        x: f32x4.splat(0.0),
        y: f32x4.splat(0.0),
        z: f32x4.splat(0.0),
        w: f32x4.splat(0.0),
    };
}

export fn f32x4x4.identity() -> f32x4x4 {
    return f32x4x4 {
        x: f32x4.new(1.0, 0.0, 0.0, 0.0),
        y: f32x4.new(0.0, 1.0, 0.0, 0.0),
        z: f32x4.new(0.0, 0.0, 1.0, 0.0),
        w: f32x4.new(0.0, 0.0, 0.0, 1.0),
    };
}

export fn f32x4x4.new(
    xx: f32, xy: f32, xz: f32, xw: f32,
    yx: f32, yy: f32, yz: f32, yw: f32,
    zx: f32, zy: f32, zz: f32, zw: f32,
    wx: f32, wy: f32, wz: f32, ww: f32,
) -> f32x4x4 {
    return f32x4x4 {
        x: f32x4.new(xx, xy, xz, xw),
        y: f32x4.new(yx, yy, yz, yw),
        z: f32x4.new(zx, zy, zz, zw),
        w: f32x4.new(wx, wy, wz, ww),
    };
}

export fn f32x4x4.rotation_x(radians: f32) -> f32x4x4 {
    let c = radians.cos();
    let s = radians.sin();
    return f32x4x4 {
        x: f32x4.new(1.0, 0.0, 0.0, 0.0),
        y: f32x4.new(0.0,   c,  -s, 0.0),
        z: f32x4.new(0.0,   s,   c, 0.0),
        w: f32x4.new(0.0, 0.0, 0.0, 1.0),
    };
}

export fn f32x4x4.rotation_y(radians: f32) -> f32x4x4 {
    let c = radians.cos();
    let s = radians.sin();
    return f32x4x4 {
        x: f32x4.new(  c, 0.0,   s, 0.0),
        y: f32x4.new(0.0, 1.0, 0.0, 0.0),
        z: f32x4.new( -s, 0.0,   c, 0.0),
        w: f32x4.new(0.0, 0.0, 0.0, 1.0),
    };
}

export fn f32x4x4.rotation_z(radians: f32) -> f32x4x4 {
    let c = radians.cos();
    let s = radians.sin();
    return f32x4x4 {
        x: f32x4.new(  c,  -s, 0.0, 0.0),
        y: f32x4.new(  s,   c, 0.0, 0.0),
        z: f32x4.new(0.0, 0.0, 1.0, 0.0),
        w: f32x4.new(0.0, 0.0, 0.0, 1.0),
    };
}

export fn f32x4x4.translation(translation: f32x4) -> f32x4x4 {
    return f32x4x4 {
        x: f32x4.new(1.0, 0.0, 0.0, translation.x),
        y: f32x4.new(0.0, 1.0, 0.0, translation.y),
        z: f32x4.new(0.0, 0.0, 1.0, translation.z),
        w: f32x4.new(0.0, 0.0, 0.0, translation.w),
    };
}

export fn f32x4x4.scaling(scale: f32x4) -> f32x4x4 {
    return f32x4x4 {
        x: f32x4.new(scale.x,     0.0,     0.0,     0.0),
        y: f32x4.new(    0.0, scale.y,     0.0,     0.0),
        z: f32x4.new(    0.0,     0.0, scale.z,     0.0),
        w: f32x4.new(    0.0,     0.0,     0.0, scale.w),
    };
}
)";
    rain::util::console_log(ANSI_CYAN, "Source code:\n", ANSI_RESET, source, "\n");

    initialize();

#define ABORT_ON_ERROR(result, msg)                                 \
    if (!result.has_value()) {                                      \
        std::cout << msg << result.error()->message() << std::endl; \
        std::abort();                                               \
    }

    auto compile_result = rain::compile(std::string_view{source.cbegin(), source.cend()});
    ABORT_ON_ERROR(compile_result, "Failed to compile: ");
    auto rain_mod = std::move(compile_result).value();

    rain_mod.optimize();

    auto ir = rain_mod.emit_ir();
    ABORT_ON_ERROR(ir, "Failed to emit IR: ");
    auto ir_bytes = std::move(ir).value();
    rain::util::console_log(ANSI_CYAN, "LLVM_IR:\n", ANSI_RESET, ir_bytes, "\n");

    auto wasm_result = rain::link(rain_mod);
    ABORT_ON_ERROR(wasm_result, "Failed to link: ");
    auto wasm_bytes = std::move(wasm_result).value();

    auto wat_result = rain::decompile(wasm_bytes->data());
    ABORT_ON_ERROR(wat_result, "Failed to decompile: ");
    auto wat_bytes = std::move(wat_result).value();

    rain::util::console_log(ANSI_CYAN, "WAT:\n", ANSI_RESET, wat_bytes->string(), "\n");

#undef ABORT_ON_ERROR
}

#endif  // !defined(__wasm__)
