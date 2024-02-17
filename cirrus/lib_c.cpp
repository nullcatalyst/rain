#include <iostream>

#include "cirrus/cirrus.h"
#include "cirrus/cirrus.hpp"

extern "C" {

bool cirrus_compile_and_print(const char* source, const int optimize) {
    auto result = cirrus::compile(source);
    if (!result.has_value()) {
        std::cerr << result.error()->message() << '\n';
        return false;
    }

    auto mod = std::move(result).value();
    if (optimize) {
        mod.optimize();
    }

    auto ir = mod.emit_ir();
    if (!ir.has_value()) {
        std::cerr << ir.error()->message() << '\n';
        return false;
    }

    std::cout << std::move(ir).value() << '\n';
    return true;
}

}  // extern "C"
