#include "rain/code/scope.hpp"

namespace rain::code {

Scope Scope::builtin_scope(llvm::LLVMContext& llvm_ctx) {
    Scope scope;

    const auto add_type = [&](const std::string_view name, llvm::Type* llvm_type) {
        const auto type = ast::OpaqueType::alloc(name);
        scope._named_types.emplace(name, type);
        scope._llvm_types.emplace(type, llvm_type);
    };

    add_type("i8", llvm::Type::getInt8Ty(llvm_ctx));
    add_type("i16", llvm::Type::getInt16Ty(llvm_ctx));
    add_type("i32", llvm::Type::getInt32Ty(llvm_ctx));
    add_type("i64", llvm::Type::getInt64Ty(llvm_ctx));
    add_type("u8", llvm::Type::getInt8Ty(llvm_ctx));
    add_type("u16", llvm::Type::getInt16Ty(llvm_ctx));
    add_type("u32", llvm::Type::getInt32Ty(llvm_ctx));
    add_type("u64", llvm::Type::getInt64Ty(llvm_ctx));
    add_type("f32", llvm::Type::getFloatTy(llvm_ctx));
    add_type("f64", llvm::Type::getDoubleTy(llvm_ctx));

    return scope;
}

llvm::Type* Scope::find_llvm_type(const ast::TypePtr& type) const noexcept {
    assert(type != nullptr);

    const auto* scope = this;
    do {
        if (const auto it = scope->_llvm_types.find(type); it != _llvm_types.end()) {
            return it->second;
        }
        scope = scope->parent();
    } while (scope != nullptr);

    return nullptr;
}

ast::TypePtr Scope::resolve_type(const ast::TypePtr type) const noexcept {
    assert(type != nullptr);
    if (type->kind() != ast::TypeKind::UnresolvedType) {
        // The type is already resolved to a known type.
        return type;
    }

    const auto  name  = static_cast<ast::UnresolvedType*>(type.get())->name();
    const auto* scope = this;
    do {
        if (const auto it = scope->_named_types.find(name); it != scope->_named_types.end()) {
            return it->second;
        }
        scope = scope->parent();
    } while (scope != nullptr);

    return nullptr;
}

const Variable* Scope::find_variable(const util::String name) const noexcept {
#if RAIN_USE_TWINE
    const auto str_name = std::string(name);
#else
    const auto str_name = name;
#endif
    if (const auto it = _variables.find(str_name); it != _variables.end()) {
        return &it->second;
    }

    if (_parent != nullptr) {
        return _parent->find_variable(name);
    }
    return nullptr;
}

}  // namespace rain::code
