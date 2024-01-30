#include "cirrus/cirrus.h"
#include "cirrus/cirrus.hpp"

namespace cirrus {

util::Result<code::Module> compile(const std::string_view source) {
    lang::Lexer  lexer(source);
    lang::Parser parser;

    auto parse_result = parser.parse(lexer);
    FORWARD_ERROR_WITH_TYPE(code::Module, parse_result);

    code::Builder::initialize_llvm();
    code::Builder builder;
    return builder.build(parse_result.unwrap());
}

}  // namespace cirrus

extern "C" {

bool cirrus_compile_and_print(const char* source, const int optimize) {
    auto result = cirrus::compile(source);
    if (result.is_error()) {
        std::cerr << result.unwrap_error()->message() << std::endl;
        return false;
    }

    if (optimize) {
        result.unwrap().optimize();
    }

    const auto ir = result.unwrap().llvm_ir();
    std::cout << ir << std::endl;
    return true;
}

}  // extern "C"
