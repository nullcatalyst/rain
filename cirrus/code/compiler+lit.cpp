#include "cirrus/code/compiler.hpp"
// ^ Keep at top

#include "cirrus/ast/expr/all.hpp"
#include "cirrus/ast/type/all.hpp"
#include "cirrus/err/simple.hpp"

namespace cirrus::code {

util::Result<llvm::Value*> Compiler::build(Context&                         ctx,
                                           const ast::IdentifierExpression& identifier_expression) {
    const Variable* const var = ctx.scope.find_variable(identifier_expression.name());
    if (var == nullptr) {
        return ERR_PTR(err::SimpleError,
                       absl::StrFormat("unknown variable: %s", identifier_expression.name()));
    }

    if (var->_alloca) {
        return _llvm_ir.CreateLoad(llvm::Type::getInt32Ty(*_llvm_ctx), var->_value);
    }

    return var->_value;
}

util::Result<llvm::Value*> Compiler::build(Context&                      ctx,
                                           const ast::IntegerExpression& integer_expression) {
    return llvm::ConstantInt::get(*_llvm_ctx, llvm::APInt(32, integer_expression.value()));
}

util::Result<llvm::Value*> Compiler::build(Context&                    ctx,
                                           const ast::FloatExpression& float_expression) {
    return llvm::ConstantFP::get(*_llvm_ctx, llvm::APFloat(float_expression.value()));
}

util::Result<llvm::Value*> Compiler::build(Context&                   ctx,
                                           const ast::CallExpression& call_expression) {
    // // TODO: Get the proper function type instead of assuming all `i32`s.
    // llvm::Type* const llvm_i32_type = llvm::Type::getInt32Ty(*_llvm_ctx);
    // llvm::Type* const llvm_f64_type = llvm::Type::getDoubleTy(*_llvm_ctx);

    // llvm::SmallVector<llvm::Type*, 4> llvm_argument_types;
    // llvm_argument_types.reserve(call_expression.arguments().size());
    // for (const auto& argument : call_expression.arguments()) {
    //     // TODO: determine the type of the argument.

    //     if (argument->kind() == ast::NodeKind::IntegerExpression) {
    //         llvm_argument_types.emplace_back(llvm_i32_type);
    //     } else if (argument->kind() == ast::NodeKind::FloatExpression) {
    //         llvm_argument_types.emplace_back(llvm_f64_type);
    //     } else {
    //         llvm_argument_types.emplace_back(llvm_i32_type);
    //     }
    // }
    // llvm::FunctionType* const llvm_function_type =
    //     llvm::FunctionType::get(llvm_i32_type, llvm_argument_types, false);

    auto callee = build(ctx, call_expression.callee());
    FORWARD_ERROR(callee);

    if (call_expression.callee()->kind() != ast::ExpressionKind::IdentifierExpression) {
        return ERR_PTR(err::SimpleError,
                       "cannot call non-identifier expression: currently not implemented");
    }

    const auto* const identifier_expression =
        static_cast<const ast::IdentifierExpression*>(call_expression.callee().get());
    const auto* const         var = ctx.scope.find_variable(identifier_expression->name());
    llvm::FunctionType* const llvm_function_type = llvm::cast<llvm::FunctionType>(var->_type);

    llvm::SmallVector<llvm::Value*, 4> llvm_arguments;
    llvm_arguments.reserve(call_expression.arguments().size());
    for (const auto& argument : call_expression.arguments()) {
        auto llvm_argument = build(ctx, argument);
        FORWARD_ERROR(llvm_argument);
        llvm_arguments.emplace_back(std::move(llvm_argument).value());
    }

    return _llvm_ir.CreateCall(llvm_function_type, std::move(callee).value(), llvm_arguments);
}

}  // namespace cirrus::code
