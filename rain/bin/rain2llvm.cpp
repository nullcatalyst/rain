#include "rain/bin/common.hpp"
#include "rain/compile.hpp"
#include "rain/lang/code/target/default.hpp"
#include "rain/util/wasm.hpp"

WASM_EXPORT("init")
void initialize() {
    rain::lang::code::initialize_llvm();
    // load_external_functions_into_llvm();
}

WASM_EXPORT("compile")
void compile(const char* source_start, const char* source_end) {
    static std::string prev_result;
    prev_result.clear();

    // Compile the source code.
    auto compile_result = rain::compile(std::string_view{source_start, source_end});
    if (!compile_result.has_value()) {
        const auto msg = compile_result.error()->message();
        callback(rain::Action::Error, msg.c_str(), msg.c_str() + msg.size());
        return;
    }
    auto module = std::move(compile_result).value();

    // Optimize the module.
    module.optimize();

    // Get the LLVM IR.
    auto ir_result = module.emit_ir();
    if (!ir_result.has_value()) {
        const auto msg = ir_result.error()->message();
        callback(rain::Action::Error, msg.c_str(), msg.c_str() + msg.size());
        return;
    }

    prev_result = std::move(ir_result).value();
    callback(rain::Action::CompileRain, prev_result.c_str(),
             prev_result.c_str() + prev_result.size());
}

#if !defined(__wasm__)

int main(const int argc, const char* const argv[]) {
    //     const std::string source = R"(
    // fn fib(n: i32) -> i32 {
    //     if n <= 1 {
    //         return n
    //     }
    //     return fib(n - 1) + fib(n - 2)
    // }

    // export fn double(n: i32) -> i32 {
    //     let n = n * 2
    //     return n
    // }

    // export fn run() -> i32 {
    //     let i = double(42)
    //     return #fib(6)
    // }
    // )";
    const std::string_view source = R"(
fn i32.double(self) -> i32 {
    2 * self
}

fn do_something() -> i32 {
    4.double()
}
)";
    rain::util::console_log(ANSI_CYAN, "Source code:\n", ANSI_RESET, source, "\n");

    initialize();
    compile(&*source.cbegin(), &*source.cend());
    return 0;
}

#endif  // !defined(__wasm__)
