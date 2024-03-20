#pragma once

#include <memory>

#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/Target/TargetMachine.h"
#include "rain/util/result.hpp"

namespace rain::lang::code {

class Module {
    std::unique_ptr<llvm::LLVMContext>   _llvm_ctx;
    std::unique_ptr<llvm::Module>        _llvm_module;
    std::unique_ptr<llvm::TargetMachine> _llvm_target_machine;

  public:
    Module();
    Module(std::unique_ptr<llvm::TargetMachine> llvm_target_machine);

    Module(const Module&)            = delete;
    Module& operator=(const Module&) = delete;

    Module(Module&&)            = default;
    Module& operator=(Module&&) = default;

    ~Module() = default;

    [[nodiscard]] constexpr const llvm::LLVMContext& llvm_context() const noexcept {
        return *_llvm_ctx;
    }
    [[nodiscard]] constexpr llvm::LLVMContext&  llvm_context() noexcept { return *_llvm_ctx; }
    [[nodiscard]] constexpr const llvm::Module& llvm_module() const noexcept {
        return *_llvm_module;
    }
    [[nodiscard]] constexpr llvm::Module& llvm_module() noexcept { return *_llvm_module; }
    [[nodiscard]] constexpr const llvm::TargetMachine& llvm_target_machine() const noexcept {
        return *_llvm_target_machine;
    }
    [[nodiscard]] constexpr llvm::TargetMachine& llvm_target_machine() noexcept {
        return *_llvm_target_machine;
    }

    void optimize();

    [[nodiscard]] util::Result<std::string>                         emit_ir() const;
    [[nodiscard]] util::Result<std::unique_ptr<llvm::MemoryBuffer>> emit_obj() const;
};

}  // namespace rain::lang::code
