#include <fstream>
#include <iostream>

#include "cirrus/cirrus.hpp"
#include "cirrus/util/colors.hpp"
#include "llvm/ExecutionEngine/GenericValue.h"
#include "llvm/Support/MemoryBuffer.h"

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

int main(const int argc, const char* const argv[]) {
    using namespace cirrus;
    code::Compiler::initialize_llvm();
    code::Compiler::use_external_function("sqrt", lle_X_sqrt);

#define ABORT_ON_ERROR(result, msg)                                            \
    if (!result.has_value()) {                                                 \
        std::cerr << COUT_COLOR_RED(msg) << result.error()->message() << '\n'; \
        return 1;                                                              \
    }

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

    code::Module mod;
    {
        lang::Lexer  lexer(source);
        lang::Parser parser;

        auto parse_result = parser.parse(lexer);
        ABORT_ON_ERROR(parse_result, "Failed to parse: ");

        code::Compiler compiler;

        {
            auto sqrt_type = compiler.get_function_type(
                std::vector<ast::TypePtr>{ast::UnresolvedType::alloc("f64")},
                ast::UnresolvedType::alloc("f64"));
            compiler.declare_external_function("sqrt", std::move(sqrt_type).value());
        }

        auto compile_result = compiler.build(std::move(parse_result).value());
        ABORT_ON_ERROR(compile_result, "Failed to compile: ");

        mod = std::move(compile_result).value();
    }

    // auto result = compile(source);
    // ABORT_ON_ERROR(result, "Failed to compile");

    {
        // Optimize the LLVM module. This is done in place.
        mod.optimize();
    }

    {
        // Print the LLVM IR. This is useful for debugging.
        auto ir = mod.emit_ir();
        ABORT_ON_ERROR(ir, "Failed to emit IR");
        std::cout << std::move(ir).value() << "\n";
    }

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

#undef ABORT_ON_ERROR
}
