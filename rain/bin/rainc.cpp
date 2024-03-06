#include "llvm/ExecutionEngine/GenericValue.h"
#include "rain/bin/common.hpp"
#include "rain/code/initialize.hpp"

#define RAIN_INCLUDE_COMPILE
#define RAIN_INCLUDE_LINK
#define RAIN_INCLUDE_DECOMPILE
#include "rain/rain.hpp"

namespace {

llvm::GenericValue lle_X_sqrt(llvm::FunctionType*                llvm_function_type,
                              llvm::ArrayRef<llvm::GenericValue> llvm_arguments) {
    if (llvm_arguments.size() != 1) {
        rain::util::console_error(ANSI_RED, "bad builtin function call: \"sqrt\": ", ANSI_RESET,
                                  "expected 1 argument, got ", llvm_arguments.size());
        std::abort();
    }

    const double value = llvm_arguments[0].DoubleVal;

    llvm::GenericValue result;
    result.DoubleVal = std::sqrt(value);
    return result;
}

}  // namespace

WASM_EXPORT("init")
void initialize() {
    rain::code::initialize_llvm();

    // Add external functions here.
    // This is required if there are any external functions that will be called at compile time.

    // rain::code::Compiler::use_external_function("sqrt", lle_X_sqrt);
}

WASM_EXPORT("compile")
void compile(const char* source_start, const char* source_end) {
    static std::string prev_result;
    prev_result.clear();

    // Compile the source code.
    auto compile_result = rain::compile(std::string_view{source_start, source_end});
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
    callback(rain::Action::CompileLLVM, wasm->string().begin(), wasm->string().begin());

    // Decompile the WebAssembly into WAT.
    auto decompile_result =
        rain::decompile(std::span{reinterpret_cast<const uint8_t*>(source_start),
                                  reinterpret_cast<const uint8_t*>(source_end)});
    if (!decompile_result.has_value()) {
        rain::throw_error(decompile_result.error()->message());
    }
    auto wat = std::move(decompile_result).value();
    callback(rain::Action::DecompileWasm, wat->string().begin(), wat->string().end());
}

#if !defined(__wasm__)

int main(const int argc, const char* const argv[]) {
    const std::string_view source = R"(
struct Point {
    x: i32;
    y: i32;
}

export fn new_point(x: i32, y: i32) -> Point {
    return Point {
        x: x,
        y: y,
    };
}

fn fib(n: i32) -> i32 {
    if n <= 1 {
        return n;
    };
    return fib(n - 1) + fib(n - 2);
}

export fn double(n: i32) -> i32 {
    let n = n * 2;
    return n;
}

export fn run() -> i32 {
    let i = double(42);
    return #fib(6);
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
