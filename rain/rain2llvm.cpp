#include <fstream>

#include "llvm/ExecutionEngine/GenericValue.h"
#include "llvm/Support/MemoryBuffer.h"
#include "rain/code/compiler.hpp"
#include "rain/code/initialize.hpp"
#include "rain/lang/lexer.hpp"
#include "rain/lang/parser.hpp"
#include "rain/util/colors.hpp"
#include "rain/util/log.hpp"
#include "rain/util/wasm.hpp"

#define ABORT_ON_ERROR(result, msg)                                                      \
    if (!result.has_value()) {                                                           \
        rain::util::console_error(ANSI_RED, msg, ANSI_RESET, result.error()->message()); \
        std::abort();                                                                    \
    }

extern "C" {

static llvm::GenericValue lle_X_sqrt(llvm::FunctionType*                llvm_function_type,
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

}  // extern "C"

WASM_IMPORT("env", "error")
void throw_error(const char* msg_start, const char* msg_end);

WASM_IMPORT("env", "callback")
void compile_callback(const char* msg_start, const char* msg_end);

WASM_EXPORT("malloc")
void* memory_allocate(size_t size) { return malloc(size); }

WASM_EXPORT("free")
void memory_free(void* ptr) { return free(ptr); }

#if defined(__wasm__)
extern "C" void __wasm_call_ctors();
#endif  // defined(__wasm__)

WASM_EXPORT("init")
void initialize() {
#if defined(__wasm__)
    __wasm_call_ctors();
#endif  // defined(__wasm__)

    rain::code::initialize_llvm();
    // rain::code::Compiler::use_external_function("sqrt", lle_X_sqrt);
}

WASM_EXPORT("compile")
void compile(const char* source_start, const char* source_end) {
    static std::string prev_result;
    prev_result.clear();

    using namespace rain;

    lang::Lexer  lexer(std::string_view{source_start, source_end});
    lang::Parser parser;

    auto parse_result = parser.parse(lexer);
    ABORT_ON_ERROR(parse_result, "Failed to parse: ");

    code::Compiler compiler;
    {
        // Declare the `sqrt` function as an external function.
        auto sqrt_type =
            compiler.get_function_type(std::vector<ast::TypePtr>{ast::UnresolvedType::alloc("f64")},
                                       ast::UnresolvedType::alloc("f64"));
        // compiler.declare_external_function("sqrt", std::move(sqrt_type).value());
    }

    auto compile_result = compiler.build(std::move(parse_result).value());
    ABORT_ON_ERROR(compile_result, "Failed to compile: ");

    rain::code::Module mod = std::move(compile_result).value();

    // Optimize the LLVM module. This is done in place.
    mod.optimize();

    auto ir = mod.emit_ir();
    ABORT_ON_ERROR(ir, "Failed to emit IR");
    prev_result = std::move(ir).value();

    compile_callback(prev_result.c_str(), prev_result.c_str() + prev_result.size());
}

#if !defined(__wasm__)

WASM_IMPORT("env", "error")
void throw_error(const char* msg_start, const char* msg_end) {
    rain::util::console_error(ANSI_RED, "error: ", ANSI_RESET,
                              std::string_view{msg_start, msg_end});
    std::abort();
}

WASM_IMPORT("env", "callback")
void compile_callback(const char* msg_start, const char* msg_end) {
    rain::util::console_log(ANSI_CYAN, "LLVM_IR:\n", ANSI_RESET,
                            std::string_view{msg_start, msg_end}, "\n");
}

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

#undef ABORT_ON_ERROR
