#pragma once

#include <cassert>

namespace rain::util {

// Implementation taken from:
// https://en.cppreference.com/w/cpp/utility/unreachable
[[noreturn]] inline void unreachable() noexcept {
    // Uses compiler specific extensions if possible.
    // Even if no extension is used, undefined behavior is still raised by
    // an empty function body and the noreturn attribute.
#if defined(_MSC_VER) && !defined(__clang__)  // MSVC
    __assume(false);
#else  // GCC, Clang
    __builtin_unreachable();
#endif
}

}  // namespace rain::util
