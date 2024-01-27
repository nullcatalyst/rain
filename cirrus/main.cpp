#include "cirrus/lang/lexer.hpp"
#include "cirrus/lang/parser.hpp"
#include "cirrus/transform/codegen.hpp"
#include "cirrus/util/colors.hpp"

int main(const int argc, const char* const argv[]) {
    using namespace cirrus;

#define HANDLE_ERROR(result)                                        \
    if (result.is_error()) {                                        \
        std::cerr << result.unwrap_error()->message() << std::endl; \
        return 1;                                                   \
    }

    // const std::string_view source = "struct Foo {\n    x: i32;\n    y: i32;\n}\n";
    const std::string_view source = R"(
fn mul(x: i32, y: i32) -> i32 {
    x * y
}
)";
    std::cout << COUT_COLOR_CYAN("\nSource code:\n") << source << '\n' << std::endl;

    lang::Lexer  lexer(source);
    lang::Parser parser;

    const auto parse_result = parser.parse(lexer);
    HANDLE_ERROR(parse_result);

    transform::Codegen codegen;
    auto               generate_result = codegen.generate(parse_result.unwrap());
    HANDLE_ERROR(generate_result);

    const auto ir = codegen.llvm_ir();
    std::cout << ir << std::endl;
    return 0;
}
