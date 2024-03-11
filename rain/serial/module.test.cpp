#include "gtest/gtest.h"
#include "rain/serial/builder.hpp"

TEST(ModuleBuilder, empty) {
    using namespace rain::serial;

    std::unqiue_ptr<Module> memory;
    {
        Builder builder;
        Module  mod = builder.build();
        memory      = mod.release();
    }
    {}
    Builder builder;
    Module  mod                = builder.build();
    auto [memory, memory_size] = mod.release();

    auto result = Module::from_memory(memory, memory_size);
    ASSERT_NOT_ERROR(result);
}
