#include "cirrus/cirrus.h"
#include "cirrus/cirrus.hpp"

namespace cirrus {

util::Result<code::Module> compile(const std::string_view source) {
    lang::Lexer  lexer(source);
    lang::Parser parser;

    auto parse_result = parser.parse(lexer);
    FORWARD_ERROR(parse_result);

    code::Compiler::initialize_llvm();
    code::Compiler compiler;
    return compiler.build(std::move(parse_result).value());
}

}  // namespace cirrus

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
