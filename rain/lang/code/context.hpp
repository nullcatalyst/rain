#pragma once

#include <memory>
#include <tuple>

#include "absl/container/flat_hash_map.h"
#include "llvm/ExecutionEngine/ExecutionEngine.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Type.h"
#include "llvm/Target/TargetMachine.h"
#include "rain/lang/ast/type/type.hpp"
#include "rain/lang/ast/var/variable.hpp"
#include "rain/lang/code/module.hpp"

namespace rain::lang::code {

class Context {
    Module& _module;
    // llvm::ExecutionEngine _engine;
    llvm::IRBuilder<> _llvm_builder;

    absl::flat_hash_map<const ast::Type*, llvm::Type*>      _llvm_types;
    absl::flat_hash_map<const ast::Variable*, llvm::Value*> _llvm_values;

  public:
    Context(Module& module) : _module(module), _llvm_builder(module.llvm_context()) {}
    Context(const Context&)            = delete;
    Context& operator=(const Context&) = delete;
    Context(Context&&)                 = delete;
    Context& operator=(Context&&)      = delete;

    [[nodiscard]] constexpr llvm::LLVMContext& llvm_context() noexcept {
        return _module.llvm_context();
    }
    [[nodiscard]] constexpr const llvm::LLVMContext& llvm_context() const noexcept {
        return _module.llvm_context();
    }

    [[nodiscard]] constexpr llvm::Module& llvm_module() noexcept { return _module.llvm_module(); }
    [[nodiscard]] constexpr const llvm::Module& llvm_module() const noexcept {
        return _module.llvm_module();
    }

    [[nodiscard]] constexpr const llvm::TargetMachine& llvm_target_machine() const noexcept {
        return _module.llvm_target_machine();
    }

    // [[nodiscard]] constexpr llvm::ExecutionEngine& llvm_engine() const noexcept { return _engine;
    // }

    [[nodiscard]] constexpr llvm::IRBuilder<>& llvm_builder() noexcept { return _llvm_builder; }

    void                      set_llvm_type(const ast::Type* type, llvm::Type* llvm_type);
    [[nodiscard]] llvm::Type* llvm_type(const ast::Type* type) const;

    void set_llvm_value(const ast::Variable* variable, llvm::Value* llvm_value);
    [[nodiscard]] llvm::Value* llvm_value(const ast::Variable* variable) const;
};

}  // namespace rain::lang::code
