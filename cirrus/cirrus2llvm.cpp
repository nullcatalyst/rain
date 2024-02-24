#include <fstream>

#include "cirrus/code/compiler.hpp"
#include "cirrus/code/initialize.hpp"
#include "cirrus/lang/lexer.hpp"
#include "cirrus/lang/parser.hpp"
#include "cirrus/util/colors.hpp"
#include "cirrus/util/log.hpp"
#include "cirrus/util/wasm.hpp"
#include "llvm/ExecutionEngine/GenericValue.h"
#include "llvm/Support/MemoryBuffer.h"

#define ABORT_ON_ERROR(result, msg)                                                        \
    if (!result.has_value()) {                                                             \
        cirrus::util::console_error(ANSI_RED, msg, ANSI_RESET, result.error()->message()); \
        std::abort();                                                                      \
    }

extern "C" {

static llvm::GenericValue lle_X_sqrt(llvm::FunctionType*                llvm_function_type,
                                     llvm::ArrayRef<llvm::GenericValue> llvm_arguments) {
    if (llvm_arguments.size() != 1) {
        cirrus::util::console_error(ANSI_RED, "bad builtin function call: \"sqrt\": ", ANSI_RESET,
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

    cirrus::code::initialize_llvm();
    // cirrus::code::Compiler::use_external_function("sqrt", lle_X_sqrt);
}

WASM_EXPORT("compile")
void compile(const char* source_start, const char* source_end) {
    static std::string prev_result;
    prev_result.clear();

    using namespace cirrus;

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
        compiler.declare_external_function("sqrt", std::move(sqrt_type).value());
    }

    auto compile_result = compiler.build(std::move(parse_result).value());
    ABORT_ON_ERROR(compile_result, "Failed to compile: ");

    cirrus::code::Module mod = std::move(compile_result).value();

    // Optimize the LLVM module. This is done in place.
    mod.optimize();

    auto ir = mod.emit_ir();
    ABORT_ON_ERROR(ir, "Failed to emit IR");
    prev_result = std::move(ir).value();

    compile_callback(prev_result.c_str(), prev_result.c_str() + prev_result.size());
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
    sqrt(16.0)
    return fib(6)
}
)";
    cirrus::util::console_log(ANSI_CYAN, "Source code:\n", ANSI_RESET, source, "\n");

    initialize();

    const auto ir_buffer = compile(&*source.cbegin(), &*source.cend());
    cirrus::util::console_log(ANSI_CYAN, "LLVM_IR:\n", ANSI_RESET,
                              std::string_view{ir_buffer.start, ir_buffer.end}, "\n");

    return 0;
}

#endif  // !defined(__wasm__)

#undef ABORT_ON_ERROR
