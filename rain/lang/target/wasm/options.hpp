#pragma once

#include <optional>
#include <span>
#include <string>
#include <vector>

#include "llvm/IR/Function.h"
#include "rain/lang/options.hpp"
#include "rain/lang/target/wasm/init.hpp"

namespace rain::lang::wasm {

class Options : public ::rain::lang::Options {
    uint32_t                 _stack_size = 0;
    std::string              _memory_export_name;
    std::vector<std::string> _interpreter_functions;

  public:
    ~Options() override;

    void set_stack_size(const uint32_t stack_size) noexcept { _stack_size = stack_size; }
    void set_memory_export_name(std::string&& memory_export_name) noexcept {
        _memory_export_name = std::move(memory_export_name);
    }

    [[nodiscard]] constexpr bool     optimize() const noexcept override { return true; }
    [[nodiscard]] constexpr uint32_t stack_size() const noexcept override { return _stack_size; }
    [[nodiscard]] constexpr std::string_view memory_export_name() const noexcept override {
        return _memory_export_name;
    }

    [[nodiscard]] std::unique_ptr<llvm::TargetMachine>   create_target_machine() override;
    [[nodiscard]] std::unique_ptr<llvm::ExecutionEngine> create_engine(
        std::unique_ptr<llvm::Module>        llvm_module,
        std::unique_ptr<llvm::TargetMachine> llvm_target_machine) override;

    [[nodiscard]] bool extern_is_compile_time_runnable(
        const std::span<const std::string> keys) override;
    void compile_extern_compile_time_runnable(code::Context& ctx, llvm::Function* llvm_function,
                                              const std::span<const std::string> keys) override;
};

}  // namespace rain::lang::wasm