#include "rain/lang/code/expr/all.hpp"

namespace rain::lang::code {

llvm::Value* compile_struct_literal(Context& ctx, ast::StructLiteralExpression& struct_literal) {
    ast::StructType& struct_type = *static_cast<ast::StructType*>(struct_literal.type());
    llvm::Type*      llvm_type   = ctx.llvm_type(&struct_type);

    std::vector<llvm::Value*> llvm_field_values;
    llvm_field_values.resize(struct_type.fields().size(), nullptr);

    // Compile the field values in the order that they are defined.
    // The values will be assigned to the struct in field order, but compiling them in the order
    // that they were written in the source code allows calling functions that have order-dependent
    // side-effects inline in the struct literal.
    for (const auto& field : struct_literal.fields()) {
        assert(field.index >= 0 && field.index < llvm_field_values.size());

        llvm::Value* field_value       = compile_any_expression(ctx, *field.value);
        llvm_field_values[field.index] = field_value;
    }

    const bool fully_initialized = std::all_of(llvm_field_values.begin(), llvm_field_values.end(),
                                               [](const auto* value) { return value != nullptr; });

    // Check if all fields are constant.
    // This is an optimization, as LLVM allows us to create a constant struct if all fields are also
    // constants. Otherwise, we have to create a struct at runtime with non-constant values.
    const bool is_constant = std::all_of(
        llvm_field_values.begin(), llvm_field_values.end(), [](const llvm::Value* value) {
            return value == nullptr || llvm::isa<const llvm::Constant>(value);
        });
    if (is_constant) {
        for (int i = 0, end = struct_type.fields().size(); i < end; ++i) {
            if (llvm_field_values[i] != nullptr) {
                // The field value is already defined.
                continue;
            }

            llvm_field_values[i] = llvm::Constant::getNullValue(
                ctx.llvm_type(struct_type.fields()[i].type.get_nonnull()));
        }

        llvm::ArrayRef<llvm::Constant*> llvm_constants(
            reinterpret_cast<llvm::Constant**>(llvm_field_values.data()), llvm_field_values.size());
        if (llvm_type->isVectorTy()) {
            return llvm::ConstantVector::get(llvm_constants);
        } else {
            return llvm::ConstantStruct::get(llvm::cast<llvm::StructType>(llvm_type),
                                             llvm_constants);
        }
    }

    auto& llvm_ir = ctx.llvm_builder();

    // Create a struct with the field values.
    llvm::Value* llvm_value = fully_initialized ? llvm::PoisonValue::get(llvm_type)
                                                : llvm::Constant::getNullValue(llvm_type);
    if (llvm_type->isVectorTy()) {
        for (size_t i = 0, end = llvm_field_values.size(); i < end; ++i) {
            llvm_value = llvm_ir.CreateInsertElement(llvm_value, llvm_field_values[i], i);
        }
    } else {
        for (size_t i = 0, end = llvm_field_values.size(); i < end; ++i) {
            llvm_value = llvm_ir.CreateInsertValue(llvm_value, llvm_field_values[i], i);
        }
    }

    return llvm_value;
}

}  // namespace rain::lang::code
