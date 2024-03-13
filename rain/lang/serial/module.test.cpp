#include "rain/lang/serial/module.hpp"

#include "gtest/gtest.h"
#include "rain/lang/serial/builder.hpp"

TEST(Module, build_and_load_empty) {
    using namespace rain::serial;

    Builder builder;
    Module  mod                = builder.build();
    auto [memory, memory_size] = mod.release();

    auto result = Module::from_memory(std::move(memory), memory_size);
    EXPECT_TRUE(check_success(result));
}
