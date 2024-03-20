#include "rain/lang/code/context.hpp"

#include "rain/lang/code/target/default.hpp"

namespace rain::lang::code {

Context::Context()
    : _llvm_ctx(std::make_unique<llvm::LLVMContext>()),
      _llvm_target_machine(target_machine()),
      _llvm_module(std::make_unique<llvm::Module>("rain", *_llvm_ctx)),
      _llvm_builder(*_llvm_ctx) {}

void Context::set_llvm_type(const ast::Type* type, llvm::Type* llvm_type) {
    _llvm_types.emplace(type, llvm_type);
}

llvm::Type* Context::llvm_type(const ast::Type* type) const {
    if (const auto it = _llvm_types.find(type); it != _llvm_types.end()) {
        return it->second;
    }
    return nullptr;
}

void Context::set_llvm_value(const ast::Variable* variable, llvm::Value* llvm_value) {
    _llvm_values.emplace(variable, llvm_value);
}

llvm::Value* Context::llvm_value(const ast::Variable* variable) const {
    if (const auto it = _llvm_values.find(variable); it != _llvm_values.end()) {
        return it->second;
    }
    return nullptr;
}

std::string Context::emit_ir() const {
    std::string              str;
    llvm::raw_string_ostream os(str);
    _llvm_module->print(os, nullptr);
    return os.str();
}

}  // namespace rain::lang::code
