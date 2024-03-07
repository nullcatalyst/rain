#pragma once

#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Module.h>

#include <memory>
#include <string>
#include <string_view>

#include "absl/container/flat_hash_map.h"
#include "rain/ast/expr/all.hpp"
#include "rain/ast/type/all.hpp"
#include "rain/lang/module.hpp"
#include "rain/util/result.hpp"

namespace rain::code {

struct Variable {
    llvm::Value* _llvm_value = nullptr;
    llvm::Type*  _llvm_type  = nullptr;
    ast::TypePtr _type;

    /// If true, the variable can be mutated. This is a compile-time property.
    bool _mutable = false;

    /// If true, _value is an alloca instruction and must be loaded before use.
    bool _alloca = false;
};

class Scope {
    absl::flat_hash_map<std::string, ast::TypePtr>    _named_types;
    absl::flat_hash_map<ast::TypePtr, llvm::Type*>    _llvm_types;
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

    [[nodiscard]] ast::TypePtr    find_named_type(const util::String name) const noexcept;
    [[nodiscard]] llvm::Type*     find_llvm_type(const ast::TypePtr& type) const noexcept;
    [[nodiscard]] ast::TypePtr    resolve_type(const ast::TypePtr type) const noexcept;
    [[nodiscard]] const Variable* find_variable(const util::String name) const noexcept;

    void set_named_type(std::string_view name, ast::TypePtr type) {
        _named_types.insert_or_assign(name, std::move(type));
    }
    void set_named_type(std::string name, ast::TypePtr type) {
        _named_types.insert_or_assign(name, std::move(type));
    }

    void set_llvm_type(ast::TypePtr type, llvm::Type* llvm_type) {
        _llvm_types.insert_or_assign(std::move(type), llvm_type);
    }

    void set_variable(std::string_view name, Variable var) {
        _variables.insert_or_assign(name, std::move(var));
    }
    void set_variable(std::string name, Variable var) {
        _variables.insert_or_assign(name, std::move(var));
    }
};

}  // namespace rain::code

// template <typename H>
// H AbslHashValue(H h, const ast::TypePtr& c) {
//     return H::combine(std::move(h), c.center_, c.radius_);
// }
