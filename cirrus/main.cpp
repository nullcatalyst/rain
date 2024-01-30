#include "cirrus/cirrus.hpp"
#include "cirrus/util/colors.hpp"

int main(const int argc, const char* const argv[]) {
    using namespace cirrus;

    const std::string_view source = R"(
fn mul(x: i32, y: i32) -> i32 {
    return x * y
}
)";
    std::cout << COUT_COLOR_CYAN("\nSource code:\n") << source << "\n\n";

    auto result = compile(source);
    if (result.is_error()) {
        std::cerr << result.unwrap_error()->message() << "\n";
        return 1;
    }

    result.unwrap().optimize();
    std::cout << result.unwrap().llvm_ir() << "\n";
    return 0;
}
