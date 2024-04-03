#pragma once

#include <cstdlib>

#include "absl/strings/str_cat.h"

#if defined(__wasm__)

#include <cstdlib>
#include <string_view>

#include "rain/util/wasm.hpp"

WASM_IMPORT("console", "log")
extern "C" void _console_log_impl(const char* msg_start, const char* msg_end);
WASM_IMPORT("console", "error")
extern "C" void _console_error_impl(const char* msg_start, const char* msg_end);

namespace rain::util {

inline void console_log(std::string_view msg) {
    _console_log_impl(msg.data(), msg.data() + msg.size());
}

template <typename... Args>
void console_log(Args&&... args) {
    const auto msg = absl::StrCat(std::forward<Args>(args)...);
    _console_log_impl(msg.data(), msg.data() + msg.size());
}

template <typename... Args>
void console_error(Args&&... args) {
    const auto msg = absl::StrCat(std::forward<Args>(args)...);
    _console_error_impl(msg.data(), msg.data() + msg.size());
}

template <typename... Args>
void panic(Args&&... args) {
    const auto msg = absl::StrCat(std::forward<Args>(args)...);
    _console_error_impl(msg.data(), msg.data() + msg.size());
    std::abort();
}

}  // namespace rain::util

#else  // defined(__wasm__)

#include <iostream>

namespace rain::util {

template <typename... Args>
void console_log(Args&&... args) {
    std::cout << absl::StrCat(std::forward<Args>(args)...) << '\n';
}

template <typename... Args>
void console_error(Args&&... args) {
    std::cerr << absl::StrCat(std::forward<Args>(args)...) << '\n';
}

template <typename... Args>
[[noreturn]] void panic(Args&&... args) {
    std::cerr << absl::StrCat(std::forward<Args>(args)...) << '\n';
    std::abort();
}

}  // namespace rain::util

#endif  // !defined(__wasm__)
