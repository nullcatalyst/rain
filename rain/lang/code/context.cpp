#include "rain/lang/code/context.hpp"

namespace rain::lang::code {

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

}  // namespace rain::lang::code
