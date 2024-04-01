#pragma once

#include <span>
#include <string>

#include "llvm/ExecutionEngine/ExecutionEngine.h"
#include "llvm/IR/Function.h"

namespace rain::lang {

namespace code {

class Context;

}  // namespace code

class Options {
  public:
    virtual ~Options() = default;

    [[nodiscard]] virtual std::unique_ptr<llvm::TargetMachine>   create_target_machine() = 0;
    [[nodiscard]] virtual std::unique_ptr<llvm::ExecutionEngine> create_engine(
        std::unique_ptr<llvm::Module>        llvm_module,
        std::unique_ptr<llvm::TargetMachine> llvm_target_machine) = 0;

    [[nodiscard]] virtual bool extern_is_compile_time_runnable(
        const std::span<const std::string> keys);
    virtual void compile_extern_compile_time_runnable(code::Context&  ctx,
                                                      llvm::Function* llvm_function,
                                                      const std::span<const std::string> keys);
};

}  // namespace rain::lang
