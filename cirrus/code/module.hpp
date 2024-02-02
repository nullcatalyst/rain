#pragma once

#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Module.h>
#include <llvm/Target/TargetMachine.h>

#include <memory>

#include "cirrus/code/scope.hpp"

namespace cirrus::code {

class Module {
    std::shared_ptr<llvm::LLVMContext>   _llvm_ctx;
    std::unique_ptr<llvm::Module>        _llvm_mod;
    std::shared_ptr<llvm::TargetMachine> _llvm_target_machine;

    Scope _exported_scope;

  public:
    Module()              = default;
    Module(const Module&) = delete;
    Module(Module&&)      = default;

    Module(std::shared_ptr<llvm::LLVMContext>   ctx,
           std::shared_ptr<llvm::TargetMachine> target_machine, std::unique_ptr<llvm::Module> mod,
           Scope exported_scope);
    ~Module() = default;

    [[nodiscard]] const llvm::LLVMContext& llvm_context() const noexcept { return *_llvm_ctx; }
    [[nodiscard]] const llvm::Module&      llvm_module() const noexcept { return *_llvm_mod; }
    [[nodiscard]] llvm::Module&            llvm_module() noexcept { return *_llvm_mod; }

    [[nodiscard]] const Scope& exported_scope() const noexcept { return _exported_scope; }
    [[nodiscard]] Scope&       exported_scope() noexcept { return _exported_scope; }

    void optimize();

    [[nodiscard]] util::Result<std::string>                         emit_ir() const;
    [[nodiscard]] util::Result<std::unique_ptr<llvm::MemoryBuffer>> emit_obj() const;
};

}  // namespace cirrus::code
