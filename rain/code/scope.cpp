#include "rain/code/scope.hpp"

namespace rain::code {

Scope Scope::builtin_scope(llvm::LLVMContext& llvm_ctx) {
    Scope scope;
    scope._types.emplace("i8", llvm::Type::getInt8Ty(llvm_ctx));
    scope._types.emplace("i16", llvm::Type::getInt16Ty(llvm_ctx));
    scope._types.emplace("i32", llvm::Type::getInt32Ty(llvm_ctx));
    scope._types.emplace("i64", llvm::Type::getInt64Ty(llvm_ctx));
    scope._types.emplace("u8", llvm::Type::getInt8Ty(llvm_ctx));
    scope._types.emplace("u16", llvm::Type::getInt16Ty(llvm_ctx));
    scope._types.emplace("u32", llvm::Type::getInt32Ty(llvm_ctx));
    scope._types.emplace("u64", llvm::Type::getInt64Ty(llvm_ctx));
    scope._types.emplace("f32", llvm::Type::getFloatTy(llvm_ctx));
    scope._types.emplace("f64", llvm::Type::getDoubleTy(llvm_ctx));
    return scope;
}

// llvm::Type* Scope::find_llvm_type(const ast::Type& type) {
//     if (const auto it = _types.find(type); it != _types.end()) {
//         return it->second;
//     }
//     return nullptr;
// }

// void Scope::set_llvm_type(const std::string_view name, llvm::Type* const llvm_type) {
//     _types.emplace(name, llvm_type);
// }

}  // namespace rain::code
