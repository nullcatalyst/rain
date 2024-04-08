#include "rain/lang/ast/type/function.hpp"

#include "rain/lang/code/type/all.hpp"

namespace rain::lang::code {

llvm::Type* compile_function_type(Context& ctx, ast::FunctionType& function_type) {
    llvm::SmallVector<llvm::Type*, 4> llvm_argument_types;
    llvm_argument_types.reserve(function_type.argument_types().size());
    for (auto* argument_type : function_type.argument_types()) {
        llvm_argument_types.push_back(get_or_compile_type(ctx, *argument_type));
    }

    llvm::Type* llvm_return_type = function_type.return_type() != nullptr
                                       ? get_or_compile_type(ctx, *function_type.return_type())
                                       : llvm::Type::getVoidTy(ctx.llvm_context());

    llvm::Type* llvm_type = llvm::FunctionType::get(llvm_return_type, llvm_argument_types, false);
    ctx.set_llvm_type(&function_type, llvm_type);

    return llvm_type;
}

}  // namespace rain::lang::code
