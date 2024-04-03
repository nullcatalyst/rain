#include "rain/lang/ast/expr/method.hpp"
#include "rain/lang/code/expr/all.hpp"

namespace rain::lang::code {

llvm::Function* compile_function_declaration(
    Context& ctx, ast::FunctionDeclarationExpression& function_declaration) {
    llvm::FunctionType* llvm_type =
        reinterpret_cast<llvm::FunctionType*>(ctx.llvm_type(function_declaration.type()));
    assert(llvm_type != nullptr && "function type not found");

    std::string name;
    if (function_declaration.kind() == serial::ExpressionKind::Method) {
        ast::MethodExpression& method = static_cast<ast::MethodExpression&>(function_declaration);
        name = absl::StrCat(method.callee_type()->name(), ".", function_declaration.name());
    } else {
        name = std::string(function_declaration.name());
    }

    llvm::Function* llvm_function =
        llvm::Function::Create(llvm_type, llvm::Function::InternalLinkage, name, ctx.llvm_module());
    if (const auto* function_variable = function_declaration.variable();
        function_variable != nullptr) {
        ctx.set_llvm_value(function_variable, llvm_function);
    }

    return llvm_function;
}

}  // namespace rain::lang::code
