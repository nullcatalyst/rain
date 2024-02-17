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
