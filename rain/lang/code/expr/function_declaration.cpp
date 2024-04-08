#include "rain/lang/ast/expr/function.hpp"
#include "rain/lang/code/expr/all.hpp"

namespace rain::lang::code {

// TODO: DRY this up with the other function_declaration compile_function_declaration
llvm::Function* compile_function_declaration(Context&               ctx,
                                             ast::FunctionVariable& function_variable) {
    // Check if the declaration has already been compiled.
    if (auto* llvm_value = ctx.llvm_value(&function_variable); llvm_value != nullptr) {
        return static_cast<llvm::Function*>(llvm_value);
    }

    auto* function_type = function_variable.function_type();
    auto* llvm_type     = static_cast<llvm::FunctionType*>(ctx.llvm_type(function_type));
    assert(llvm_type != nullptr && "function type not found");

    std::string name;
    if (function_type->callee_type() != nullptr) {
        name = absl::StrCat(function_type->callee_type()->display_name(), ".",
                            function_variable.name());
    } else {
        name = std::string(function_variable.name());
    }

    llvm::Function* llvm_function =
        llvm::Function::Create(llvm_type, llvm::Function::InternalLinkage, name, ctx.llvm_module());
    ctx.set_llvm_value(&function_variable, llvm_function);

    return llvm_function;
}

llvm::Function* compile_function_declaration(
    Context& ctx, ast::FunctionDeclarationExpression& function_declaration) {
    // Check if the declaration has already been compiled.
    if (const auto* function_variable = function_declaration.variable();
        function_variable != nullptr) {
        if (auto* llvm_value = ctx.llvm_value(function_variable); llvm_value != nullptr) {
            return static_cast<llvm::Function*>(llvm_value);
        }
    }

    auto* function_type = function_declaration.function_type();
    auto* llvm_type     = static_cast<llvm::FunctionType*>(ctx.llvm_type(function_type));
    assert(llvm_type != nullptr && "function type not found");

    std::string name;
    if (function_type->callee_type() != nullptr) {
        name = absl::StrCat(function_type->callee_type()->display_name(), ".",
                            function_declaration.name());
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
