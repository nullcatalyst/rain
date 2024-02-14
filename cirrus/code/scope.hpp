#pragma once

#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Module.h>

#include <memory>
#include <string>
#include <string_view>

#include "absl/container/flat_hash_map.h"
#include "cirrus/ast/expr/all.hpp"
#include "cirrus/ast/node.hpp"
#include "cirrus/ast/type/all.hpp"
#include "cirrus/lang/module.hpp"
#include "cirrus/util/result.hpp"

namespace cirrus::code {

struct Variable {
    llvm::Value* _value = nullptr;
    llvm::Type*  _type  = nullptr;

    /// If true, the variable can be mutated. This is a compile-time property
    bool _mutable = false;

    /// If true, _value is an alloca instruction and must be loaded before use
    bool _alloca = false;
};

class Scope {
    absl::flat_hash_map<std::string, llvm::Type*>     _types;
    absl::flat_hash_map<std::string, llvm::Function*> _functions;
    absl::flat_hash_map<std::string, Variable>        _variables;

    Scope* _parent         = nullptr;
    bool   _return_allowed = false;
    bool   _break_allowed  = false;

  public:
    static Scope builtin_scope(llvm::LLVMContext& llvm_ctx);

    Scope() = default;
    explicit Scope(Scope* parent) : _parent(parent) {}
    ~Scope() = default;

    [[nodiscard]] bool   has_parent() const noexcept { return _parent != nullptr; }
    [[nodiscard]] Scope* parent() const noexcept { return _parent; }
    void                 set_parent(Scope* const parent) noexcept { _parent = parent; }

    [[nodiscard]] bool return_allowed() const noexcept { return _return_allowed; }
    void set_return_allowed(const bool allowed) noexcept { _return_allowed = allowed; }

    [[nodiscard]] bool break_allowed() const noexcept { return _break_allowed; }
    void               set_break_allowed(const bool allowed) noexcept { _break_allowed = allowed; }

    llvm::Type* find_llvm_type(const std::string_view type_name) {
        if (const auto it = _types.find(type_name); it != _types.end()) {
            return it->second;
        }

        if (_parent != nullptr) {
            return _parent->find_llvm_type(type_name);
        }
        return nullptr;
    }

    void set_llvm_type(std::string_view name, llvm::Type* const llvm_type) {
        _types.emplace(std::string(name), llvm_type);
    }

    const Variable* find_variable(const util::String name) {
#if CIRRUS_USE_TWINE
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

    void set_variable(std::string_view name, Variable var) {
        _variables.insert_or_assign(std::string(name), std::move(var));
    }
};

}  // namespace cirrus::code
