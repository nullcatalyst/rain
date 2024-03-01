#include <string_view>

#include "llvm/Support/MemoryBuffer.h"
#include "rain/code/compiler.hpp"
#include "rain/err/simple.hpp"
#include "rain/lang/lexer.hpp"
#include "rain/lang/parser.hpp"
#include "rain/util/colors.hpp"
#include "rain/util/log.hpp"
#include "rain/util/result.hpp"

#define RAIN_INCLUDE_COMPILE
#include "rain/rain.hpp"

namespace rain {

util::Result<rain::code::Module> compile(const std::string_view source) {
    lang::Lexer  lexer{source};
    lang::Parser parser;

    auto parse_result = parser.parse(lexer);
    FORWARD_ERROR(parse_result);

    code::Compiler compiler;
    return compiler.build(std::move(parse_result).value());
}

util::Result<rain::code::Module> compile(const std::string_view                    source,
                                         llvm::function_ref<void(code::Compiler&)> init_compiler) {
    lang::Lexer  lexer{source};
    lang::Parser parser;

    auto parse_result = parser.parse(lexer);
    FORWARD_ERROR(parse_result);

    code::Compiler compiler;
    init_compiler(compiler);
    return compiler.build(std::move(parse_result).value());
}

}  // namespace rain
