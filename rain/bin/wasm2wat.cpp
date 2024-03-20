#include "rain/bin/common.hpp"
#include "rain/decompile.hpp"

WASM_EXPORT("init")
void initialize() {
    // Do nothing. Nothing additional to initialize.
}

WASM_EXPORT("decompile")
void decompile(const char* source_start, const char* source_end) {
    static std::unique_ptr<rain::Buffer> prev_result;
    prev_result.reset();

    // Decompile the WebAssembly into WAT.
    auto decompile_result =
        rain::decompile(std::span{reinterpret_cast<const uint8_t*>(source_start),
                                  reinterpret_cast<const uint8_t*>(source_end)});
    if (!decompile_result.has_value()) {
        const auto msg = decompile_result.error()->message();
        callback(rain::Action::Error, msg.c_str(), msg.c_str() + msg.size());
        return;
    }

    prev_result = std::move(decompile_result).value();
    callback(rain::Action::DecompileWasm, prev_result->string().begin(),
             prev_result->string().end());
}

#if !defined(__wasm__)

#include <fstream>

int main(const int argc, const char* const argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <wasm file>" << std::endl;
        return 1;
    }

    std::ifstream file(argv[1], std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file: " << argv[1] << std::endl;
        return 1;
    }

    std::vector<uint8_t> contents;
    file.seekg(0, std::ios::end);
    contents.resize(file.tellg());
    file.seekg(0, std::ios::beg);
    file.read(reinterpret_cast<char*>(contents.data()), contents.size());

    initialize();

    decompile(reinterpret_cast<const char*>(contents.data()),
              reinterpret_cast<const char*>(contents.data() + contents.size()));
}

#endif  // !defined(__wasm__)
