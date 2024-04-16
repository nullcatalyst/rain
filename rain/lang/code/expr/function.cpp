#include "rain/lang/ast/expr/function.hpp"

#include "rain/lang/code/expr/all.hpp"
#include "rain/lang/code/type/all.hpp"

namespace rain::lang::code {

llvm::Function* compile_function(Context& ctx, ast::FunctionExpression& function) {
    llvm::Function* llvm_function =
        static_cast<llvm::Function*>(ctx.llvm_value(function.variable()));
    assert(llvm_function != nullptr && "Function not found in context");

    auto& llvm_ir    = ctx.llvm_builder();
    auto* prev_block = llvm_ir.GetInsertBlock();

    llvm::BasicBlock* llvm_block =
        llvm::BasicBlock::Create(ctx.llvm_context(), "entry", llvm_function);

    llvm_ir.SetInsertPoint(llvm_block);
    for (int i = 0; i < function.arguments().size(); ++i) {
        auto& argument = function.arguments()[i];
        ctx.set_llvm_value(argument, llvm_function->arg_begin() + i);
    }

    llvm::Value* llvm_return_value = compile_block(ctx, *function.block());
    if (auto* function_return_type = function.function_type()->return_type();
        function_return_type == nullptr) {
        llvm_ir.CreateRetVoid();
    } else {
        ast::Type* block_result_type = function.block()->type();

        do {
            if (function_return_type->kind() == serial::TypeKind::Optional) {
                auto* llvm_return_type = get_or_compile_type(ctx, *function_return_type);
                if (llvm_return_value == nullptr) {
                    llvm_return_value = llvm::Constant::getNullValue(llvm_return_type);
                } else if (llvm_return_value->getType() != llvm_return_type) {
                    // The return value is not already an optional, so we need to wrap it in an
                    // optional.
                    assert(llvm::isa<llvm::StructType>(llvm_return_type) &&
                           "Expected optional return type to be a struct");

                    const std::array<llvm::Constant*, 2> llvm_optional_values{
                        llvm::UndefValue::get(llvm_return_type->getStructElementType(0)),
                        llvm::ConstantInt::get(ctx.llvm_context(), llvm::APInt(1, 1)),
                    };
                    llvm_return_value = ctx.llvm_builder().CreateInsertValue(
                        llvm::ConstantStruct::get(static_cast<llvm::StructType*>(llvm_return_type),
                                                  llvm_optional_values),
                        llvm_return_value, 0);
                }
                break;
            }

            // Check if dereferencing the return value is necessary.
            if (block_result_type != nullptr &&
                block_result_type->kind() == serial::TypeKind::Reference &&
                block_result_type != function_return_type) {
                llvm_return_value = ctx.llvm_builder().CreateLoad(
                    get_or_compile_type(ctx, *function_return_type), llvm_return_value);
                break;
            }
        } while (false);

        llvm_ir.CreateRet(llvm_return_value);
    }

    llvm_ir.SetInsertPoint(prev_block);

    return llvm_function;
}

}  // namespace rain::lang::code
