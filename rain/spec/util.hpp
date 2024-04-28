#pragma once

#include <string_view>

#include "gtest/gtest.h"

// This must be included after gtest.h because the util::Result class checks if gtest was included
// in order to add additional functionality.
#include "rain/lang/target/wasm/init.hpp"
#include "rain/lang/target/wasm/options.hpp"
#include "rain/rain.hpp"

#define DO_OPTIMIZE true
#define DO_PRINT false

#define EXPECT_COMPILE_SUCCESS($code)                                            \
    do {                                                                         \
        rain::lang::wasm::initialize_llvm();                                     \
                                                                                 \
        rain::lang::wasm::Options options;                                       \
        auto                      module_result = rain::compile($code, options); \
        ASSERT_TRUE(check_success(module_result));                               \
        auto mod = std::move(module_result).value();                             \
                                                                                 \
        if (DO_OPTIMIZE) {                                                       \
            mod.optimize();                                                      \
        }                                                                        \
                                                                                 \
        auto ir_result = mod.emit_ir();                                          \
        ASSERT_TRUE(check_success(ir_result));                                   \
        auto ir = std::move(ir_result).value();                                  \
                                                                                 \
        if (DO_PRINT) {                                                          \
            std::cout << ir << std::endl;                                        \
        }                                                                        \
    } while (false)

#define EXPECT_COMPILE_ERROR($code)                                              \
    do {                                                                         \
        rain::lang::wasm::initialize_llvm();                                     \
                                                                                 \
        rain::lang::wasm::Options options;                                       \
        auto                      module_result = rain::compile($code, options); \
        ASSERT_FALSE(check_success(module_result));                              \
    } while (false)
