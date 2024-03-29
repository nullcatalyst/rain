#include "rain/lang/code/compile/all.hpp"

namespace rain::lang::code {

void compile_module(Context& ctx, ast::Module& module) {
    ast::BuiltinScope& builtin = *module.scope().builtin();

    ctx.set_llvm_type(builtin.bool_type(), llvm::Type::getInt1Ty(ctx.llvm_context()));
    ctx.set_llvm_type(builtin.i32_type(), llvm::Type::getInt32Ty(ctx.llvm_context()));
    ctx.set_llvm_type(builtin.i64_type(), llvm::Type::getInt64Ty(ctx.llvm_context()));
    ctx.set_llvm_type(builtin.f32_type(), llvm::Type::getFloatTy(ctx.llvm_context()));
    ctx.set_llvm_type(builtin.f64_type(), llvm::Type::getDoubleTy(ctx.llvm_context()));

    for (auto& type : builtin.owned_types()) {
        compile_type(ctx, *type);
    }

    for (auto& external_function : builtin.external_functions()) {
        auto* llvm_function_type =
            reinterpret_cast<llvm::FunctionType*>(ctx.llvm_type(external_function->type()));

        auto* llvm_function =
            llvm::Function::Create(llvm_function_type, llvm::Function::ExternalLinkage,
                                   external_function->name(), ctx.llvm_module());

        llvm_function->addFnAttr(llvm::Attribute::get(ctx.llvm_context(), "wasm-import-module",
                                                      external_function->wasm_namespace()));
        llvm_function->addFnAttr(llvm::Attribute::get(ctx.llvm_context(), "wasm-import-name",
                                                      external_function->wasm_function_name()));

        ctx.set_llvm_value(external_function.get(), llvm_function);
    }

    for (auto& type : module.scope().owned_types()) {
        compile_type(ctx, *type);
    }

    for (const auto& expression : module.expressions()) {
        switch (expression->kind()) {
            case serial::ExpressionKind::Type:
                // compile_type(ctx, static_cast<ast::TypeExpression&>(*expression));
                break;

            case serial::ExpressionKind::Function:
                compile_function(ctx, static_cast<ast::FunctionExpression&>(*expression));
                break;

            case serial::ExpressionKind::Export:
                compile_export(ctx, static_cast<ast::ExportExpression&>(*expression));
                break;

            default:
                util::console_error(
                    "failed to compile top-level expression: unknown expression kind: ",
                    static_cast<int>(expression->kind()));
                std::abort();
        }
    }
}

}  // namespace rain::lang::code
