#pragma once

#include <span>
#include <string>
#include <vector>

#include "llvm/IR/Function.h"
#include "rain/lang/options.hpp"
#include "rain/lang/target/wasm/init.hpp"

namespace rain::lang::wasm {

class Options : public ::rain::lang::Options {
    std::vector<std::string> _interpreter_functions;

  public:
    ~Options() override;

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