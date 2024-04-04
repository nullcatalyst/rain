#pragma once

#include <string_view>

#include "llvm/IR/Value.h"
#include "rain/lang/ast/var/function.hpp"

namespace rain::lang::ast {

template <typename BuildFn>
class BuiltinFunctionVariable : public FunctionVariable {
    // This is a lambda function that has the following signature:
    // llvm::Value* (*)(code::Context&, const llvm::SmallVector<llvm::Value*, 4>&)
    BuildFn _build_fn;

  public:
    BuiltinFunctionVariable(std::string_view name, FunctionType* type, BuildFn&& build_fn)
        : FunctionVariable(name, std::move(type)), _build_fn(std::move(build_fn)) {}
    ~BuiltinFunctionVariable() override = default;

    [[nodiscard]] llvm::Value* build_call(
        code::Context& ctx, const llvm::ArrayRef<llvm::Value*> arguments) const noexcept override {
        return _build_fn(ctx, arguments);
    }
};

/**
 * It is difficult to use `std::make_unique<BuiltinFunctionVariable< __what_goes_here?__ >>(...)`
 * with a lambda function as a template argument, as it typically involves having to create a
 * variable that stores the lambda, then using `decltype(...)` with that variable passed in. This
 * function is provided to make it easier, avoiding all that.
 */
template <typename BuildFn>
std::unique_ptr<BuiltinFunctionVariable<BuildFn>> make_builtin_function_variable(
    std::string_view name, FunctionType* type, BuildFn&& build_fn) {
    return std::make_unique<BuiltinFunctionVariable<BuildFn>>(name, type, std::move(build_fn));
}

}  // namespace rain::lang::ast
