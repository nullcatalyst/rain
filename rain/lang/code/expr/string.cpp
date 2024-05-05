#include "rain/lang/ast/expr/string.hpp"

#include <array>

#include "llvm/IR/Constants.h"
#include "rain/lang/code/context.hpp"

namespace rain::lang::code {

llvm::Value* compile_string(Context& ctx, ast::StringExpression& string) {
    auto& llvm_ir = ctx.llvm_builder();

    auto* llvm_string =
        llvm::ConstantDataArray::getString(ctx.llvm_context(), string.value(), false);
    auto* llvm_string_global =
        new llvm::GlobalVariable(ctx.llvm_module(), llvm_string->getType(), true,
                                 llvm::GlobalValue::InternalLinkage, llvm_string, "const.string");

    auto* llvm_begin_ptr = llvm::ConstantExpr::getGetElementPtr(
        llvm_ir.getInt8Ty(), llvm_string_global, llvm_ir.getInt32(0));
    auto* llvm_end_ptr = llvm::ConstantExpr::getGetElementPtr(
        llvm_ir.getInt8Ty(), llvm_string_global, llvm_ir.getInt32(string.value().size()));

    std::array<llvm::Constant*, 2> llvm_fields{llvm_begin_ptr, llvm_end_ptr};
    return llvm::ConstantStruct::get(static_cast<llvm::StructType*>(ctx.llvm_type(string.type())),
                                     llvm_fields);
}

}  // namespace rain::lang::code
