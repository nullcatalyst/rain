#include <fstream>
#include <iostream>

#include "cirrus/cirrus.hpp"
#include "cirrus/util/colors.hpp"
#include "llvm/Support/MemoryBuffer.h"

int main(const int argc, const char* const argv[]) {
    using namespace cirrus;

#define ABORT_ON_ERROR(result)                                 \
    if (result.is_error()) {                                   \
        std::cerr << result.unwrap_error()->message() << "\n"; \
        return 1;                                              \
    }

    const std::string_view source = R"(
    fn demo_function(x: i32, y: i32) -> i32 {
        return x * y
    }

    export fn main() -> i32 {
        return demo_function(2, 3)
    }
    )";
    std::cout << COUT_COLOR_CYAN("\nSource code:\n") << source << "\n\n";

    auto result = compile(source);
    ABORT_ON_ERROR(result);

    {
        // Optimize the LLVM module. This is done in place.
        result.unwrap().optimize();
    }

    {
        // Print the LLVM IR. This is useful for debugging.
        auto ir = result.unwrap().emit_ir();
        ABORT_ON_ERROR(ir);
        std::cout << ir.unwrap() << "\n";
    }

    auto obj = result.unwrap().emit_obj();
    ABORT_ON_ERROR(obj);

    {
        // Link the modules into a single `.wasm` file.
        code::Linker linker;
        linker.add(std::move(obj.unwrap()));

        auto wasm = linker.link();
        ABORT_ON_ERROR(wasm);

        std::ofstream out_file("/Users/scott/git/cirrus/out.wasm", std::ios::binary);
        out_file.write(wasm.unwrap()->getBufferStart(), wasm.unwrap()->getBufferSize());
    }

    return 0;

#undef ABORT_ON_ERROR
}
