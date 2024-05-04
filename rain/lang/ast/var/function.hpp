#pragma once

#include <span>
#include <string_view>

#include "llvm/ADT/ArrayRef.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Value.h"
#include "rain/lang/ast/type/function.hpp"
#include "rain/lang/ast/var/variable.hpp"

namespace rain::lang::code {

class Context;

}  // namespace rain::lang::code

namespace rain::lang::ast {

class FunctionVariable : public Variable {
    std::string_view             _name;
    absl::Nonnull<FunctionType*> _function_type;

    lex::Location _location;

  public:
    FunctionVariable(const std::string_view name, absl::Nonnull<FunctionType*> type,
                     lex::Location location)
        : _name(name), _function_type(type), _location(location) {}
    ~FunctionVariable() override = default;

    // Variable
    [[nodiscard]] serial::VariableKind kind() const noexcept override {
        return serial::VariableKind::Function;
    }
    [[nodiscard]] std::string_view     name() const noexcept override { return _name; }
    [[nodiscard]] absl::Nonnull<Type*> type() const noexcept override { return _function_type; }
    [[nodiscard]] absl::Nonnull<FunctionType*> function_type() const noexcept {
        return _function_type;
    }
    [[nodiscard]] lex::Location location() const noexcept override { return _location; }

    // FunctionVariable
    [[nodiscard]] virtual constexpr bool is_builtin() const noexcept { return false; }

    [[nodiscard]] virtual llvm::Value* build_call(
        code::Context& ctx, const llvm::ArrayRef<llvm::Value*> arguments) const noexcept;

    [[nodiscard]] util::Result<void> validate(Options& options, Scope& scope) noexcept override;
};

}  // namespace rain::lang::ast
