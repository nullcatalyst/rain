#include "rain/lang/code/compile/all.hpp"

namespace rain::lang::code {

llvm::Value* compile_array_literal(Context& ctx, ast::ArrayLiteralExpression& array_literal) {
    ast::ArrayType&  array_type = *static_cast<ast::ArrayType*>(array_literal.type());
    llvm::ArrayType* llvm_type  = static_cast<llvm::ArrayType*>(ctx.llvm_type(&array_type));

    std::vector<llvm::Value*> llvm_element_values;
    llvm_element_values.resize(array_literal.elements().size(), nullptr);

    // Compile the field values in the order that they are defined.
    // The values will be assigned to the struct in field order, but compiling them in the order
    // that they were written in the source code allows calling functions that have order-dependent
    // side-effects inline in the struct literal.
    for (int i = 0, end = llvm_element_values.size(); i < end; ++i) {
        llvm_element_values[i] = compile_any_expression(ctx, *array_literal.elements()[i]);
    }

    // Check if all fields are constant.
    // This is an optimization, as LLVM allows us to create a constant struct if all fields are also
    // constants. Otherwise, we have to create a struct at runtime with non-constant values.
    const bool is_constant = std::all_of(
        llvm_element_values.begin(), llvm_element_values.end(), [](const llvm::Value* value) {
            return value == nullptr || llvm::isa<const llvm::Constant>(value);
        });
    if (is_constant) {
        llvm::ArrayRef<llvm::Constant*> llvm_constants(
            reinterpret_cast<llvm::Constant**>(llvm_element_values.data()),
            llvm_element_values.size());
        return llvm::ConstantArray::get(llvm_type, llvm_constants);
    }

    auto& llvm_ir = ctx.llvm_builder();

    // // Create a runtime array.
    llvm::Value* llvm_array = llvm_ir.CreateAlloca(llvm_type, nullptr, "array");
    for (int i = 0, end = llvm_element_values.size(); i < end; ++i) {
        std::array<llvm::Value*, 2> indices{
            llvm::ConstantInt::get(ctx.llvm_context(), llvm::APInt(32, 0)),
            llvm::ConstantInt::get(ctx.llvm_context(), llvm::APInt(32, i)),
        };
        llvm_ir.CreateStore(llvm_element_values[i],
                            llvm_ir.CreateInBoundsGEP(llvm_type, llvm_array, indices));
    }

    return llvm_ir.CreateLoad(llvm_type, llvm_array);
}

}  // namespace rain::lang::code
