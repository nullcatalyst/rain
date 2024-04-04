#include "rain/lang/ast/var/function.hpp"

#include "rain/lang/code/context.hpp"

namespace rain::lang::ast {

llvm::Value* FunctionVariable::build_call(
    code::Context& ctx, const llvm::ArrayRef<llvm::Value*> arguments) const noexcept {
    return ctx.llvm_builder().CreateCall(
        static_cast<llvm::FunctionType*>(ctx.llvm_type(_function_type)), ctx.llvm_value(this),
        llvm::ArrayRef<llvm::Value*>(arguments.data(), arguments.size()));
}

[[nodiscard]] util::Result<void> FunctionVariable::validate(Options& options,
                                                            Scope&   scope) noexcept {
    auto resolved_type = _function_type->resolve(options, scope);
    FORWARD_ERROR(resolved_type);

    _function_type = reinterpret_cast<FunctionType*>(std::move(resolved_type).value());
    return {};
}

}  // namespace rain::lang::ast
