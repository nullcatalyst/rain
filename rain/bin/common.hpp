#pragma once

#include <cstddef>  // size_t
#include <cstdlib>  // malloc, free
#include <string_view>

#include "rain/util/colors.hpp"
#include "rain/util/log.hpp"
#include "rain/util/wasm.hpp"

namespace rain {

enum Action {
    Error     = -1,
    Undefined = 0,
    CompileRain,
    CompileLLVM,
    DecompileWasm,
};

WASM_IMPORT("env", "callback")
void callback(uint32_t action, const char* msg_start, const char* msg_end);

#if !defined(__wasm__)

void callback(uint32_t action, const char* msg_start, const char* msg_end) {
    util::console_log(ANSI_CYAN, "LLVM_IR:\n", ANSI_RESET, std::string_view{msg_start, msg_end},
                      "\n");
}

#endif  // !defined(__wasm__)

WASM_EXPORT("malloc")
void* memory_allocate(size_t size) { return malloc(size); }

WASM_EXPORT("free")
void memory_free(void* ptr) { return free(ptr); }

#if defined(__wasm__)
extern "C" void __wasm_call_ctors();
#endif  // defined(__wasm__)

}  // namespace rain
