#include <fstream>
#include <iostream>

#include "cirrus/cirrus.hpp"
#include "cirrus/util/colors.hpp"
#include "cirrus/util/wasm.hpp"
#include "llvm/ExecutionEngine/GenericValue.h"
#include "llvm/Support/MemoryBuffer.h"

#define ABORT_ON_ERROR(result, msg)                                            \
    if (!result.has_value()) {                                                 \
        std::cerr << COUT_COLOR_RED(msg) << result.error()->message() << '\n'; \
        __builtin_unreachable();                                               \
    }

extern "C" {

static llvm::GenericValue lle_X_sqrt(llvm::FunctionType*                llvm_function_type,
                                     llvm::ArrayRef<llvm::GenericValue> llvm_arguments) {
    if (llvm_arguments.size() != 1) {
        std::cerr << "sqrt(): expected 1 argument, got " << llvm_arguments.size() << '\n';
        std::abort();
    }

    const double value = llvm_arguments[0].DoubleVal;

    llvm::GenericValue result;
    result.DoubleVal = std::sqrt(value);
    return result;
}

}  // extern "C"

namespace {

cirrus::code::Module compile_from_source(const std::string_view source) {
    using namespace cirrus;

    lang::Lexer  lexer(source);
    lang::Parser parser;

    auto parse_result = parser.parse(lexer);
    ABORT_ON_ERROR(parse_result, "Failed to parse: ");

    code::Compiler compiler;

    {
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

    return mod;
}

}  // namespace

WASM_EXPORT("malloc") void* memory_allocate(size_t size) { return malloc(size); }

WASM_EXPORT("free") void memory_free(void* ptr) { return free(ptr); }

WASM_EXPORT("initialize") void initialize() {
    cirrus::code::Compiler::initialize_llvm();
    cirrus::code::Compiler::use_external_function("sqrt", lle_X_sqrt);
}

struct Buffer {
    const void* start;
    const void* end;
};

WASM_EXPORT("compile") void compile(const char* source_start, const char* source_end) {
    auto mod = compile_from_source(std::string_view{source_start, source_end});

    // Print the LLVM IR. This is useful for debugging.
    auto ir = mod.emit_ir();
    ABORT_ON_ERROR(ir, "Failed to emit IR");
    std::cout << std::move(ir).value() << "\n";

    // return Buffer{source_start, source_end};
}

int main(const int argc, const char* const argv[]) {
    const std::string source = R"(
fn fib(n: i32) -> i32 {
    if n <= 1 {
        return n
    }
    return fib(n - 1) + fib(n - 2)
}

// Iterative fib
// fn fib2(n: i32) -> i32 {
//     let mut a = 0
//     let mut b = 1
//     for _ in 0..n {
//         let tmp = a
//         a = b
//         b = tmp + b
//     }
//     return a
// }

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
    std::cout << COUT_COLOR_CYAN("\nSource code:\n") << source << "\n\n";

    initialize();
    compile(&*source.cbegin(), &*source.cend());

    // auto obj = result.unwrap().emit_obj();
    // ABORT_ON_ERROR(obj, "Failed to emit object file");

    // {
    //     // Link the modules into a single `.wasm` file.
    //     code::Linker linker;
    //     linker.add(std::move(obj.unwrap()));

    //     auto wasm = linker.link();
    //     ABORT_ON_ERROR(wasm);

    //     std::ofstream out_file("/Users/scott/git/cirrus/out.wasm", std::ios::binary);
    //     out_file.write(wasm.unwrap()->getBufferStart(), wasm.unwrap()->getBufferSize());
    // }

    return 0;
}

#undef ABORT_ON_ERROR
