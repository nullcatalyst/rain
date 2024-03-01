#include "llvm/ExecutionEngine/GenericValue.h"
#include "rain/bin/common.hpp"
#include "rain/code/initialize.hpp"

#define RAIN_INCLUDE_COMPILE
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

namespace rain {

WASM_EXPORT("init")
void initialize() {
    code::initialize_llvm();

    // Add external functions here.
    // This is required if there are any external functions that will be called at compile time.

    // rain::code::Compiler::use_external_function("sqrt", lle_X_sqrt);
}

}  // namespace rain

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

    prev_result = std::move(ir_result).value();
    callback(rain::Action::CompileRain, prev_result.c_str(),
             prev_result.c_str() + prev_result.size());
}

#if !defined(__wasm__)

int main(const int argc, const char* const argv[]) {
    const std::string source = R"(fn fib(n: i32) -> i32 {
    if n <= 1 {
        return n
    }
    return fib(n - 1) + fib(n - 2)
}

export fn double(n: i32) -> i32 {
    let n = n * 2
    return n
}

export fn run() -> i32 {
    let i = double(42)
    return #fib(6)
}
)";
    rain::util::console_log(ANSI_CYAN, "Source code:\n", ANSI_RESET, source, "\n");

    initialize();
    compile(&*source.cbegin(), &*source.cend());
    return 0;
}

#endif  // !defined(__wasm__)
