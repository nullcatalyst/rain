#pragma once

#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Module.h>

#include <memory>

#include "cirrus/code/scope.hpp"

namespace cirrus::code {

class Module {
    std::shared_ptr<llvm::LLVMContext> _ctx;
    std::unique_ptr<llvm::Module>      _mod;

    Scope _exported_scope;

  public:
    Module()              = default;
    Module(const Module&) = delete;
    Module(Module&&)      = default;

    Module(std::shared_ptr<llvm::LLVMContext> ctx, const std::string_view name);
    Module(std::shared_ptr<llvm::LLVMContext> ctx) : Module(std::move(ctx), "cirrus") {}
    ~Module() = default;

    [[nodiscard]] const llvm::LLVMContext& llvm_context() const noexcept { return *_ctx; }
    [[nodiscard]] const llvm::Module&      llvm_module() const noexcept { return *_mod; }
    [[nodiscard]] llvm::Module&            llvm_module() noexcept { return *_mod; }

    [[nodiscard]] const Scope& exported_scope() const noexcept { return _exported_scope; }
    [[nodiscard]] Scope&       exported_scope() noexcept { return _exported_scope; }

    void                      optimize();
    [[nodiscard]] std::string llvm_ir() const;
};

}  // namespace cirrus::code
