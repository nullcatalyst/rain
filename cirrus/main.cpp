#include <fstream>
#include <iostream>

#include "cirrus/cirrus.hpp"
#include "cirrus/util/colors.hpp"
#include "llvm/Support/MemoryBuffer.h"

int main(const int argc, const char* const argv[]) {
    using namespace cirrus;

#define ABORT_ON_ERROR(result, msg)                     \
    if (!result.has_value()) {                          \
        std::cerr << COUT_COLOR_RED(msg << '\n');       \
        std::cerr << result.error()->message() << "\n"; \
        return 1;                                       \
    }

    const std::string_view source = R"(
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

    export fn main() -> i32 {
        let i = double(42)
        return #fib(6)
    }
    )";
    std::cout << COUT_COLOR_CYAN("\nSource code:\n") << source << "\n\n";

    auto result = compile(source);
    ABORT_ON_ERROR(result, "Failed to compile");

    auto mod = std::move(result).value();
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
