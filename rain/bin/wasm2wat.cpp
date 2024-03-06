#include "rain/bin/common.hpp"

#define RAIN_INCLUDE_DECOMPILE
#include "rain/rain.hpp"

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
        rain::throw_error(decompile_result.error()->message());
    }

    prev_result = std::move(decompile_result).value();
    callback(rain::Action::DecompileWasm, prev_result->string().begin(),
             prev_result->string().end());
}

#if !defined(__wasm__)

int main(const int argc, const char* const argv[]) {}

#endif  // !defined(__wasm__)
