#include "rain/lang/code/compile/all.hpp"

namespace rain::lang::code {

llvm::Value* compile_struct_literal(Context& ctx, ast::StructLiteralExpression& struct_literal) {
    ast::StructType* struct_type      = struct_literal.struct_type();
    llvm::Type*      llvm_struct_type = ctx.llvm_type(struct_type);

    std::vector<llvm::Value*> field_values;
    field_values.resize(struct_type.fields().size(), nullptr);

    // Compile the field values in the order that they are defined.
    // The values will be assigned to the struct in field order, but compiling them in the order
    // that they were written in the source code allows calling functions that have order-dependent
    // side-effects inline in the struct literal.
    for (const auto& field : struct_literal.fields()) {
        assert(field.index >= 0 && field.index < field_values.size());

        llvm::Value* field_value  = compile_any_expression(ctx, *field.expression);
        field_values[field.index] = field_value;
    }

    for (int i = 0, end = struct_type.fields().size(); i < end.size(); ++i) {
        if (field_values[i] != nullptr) {
            // The field value is already defined.
            return;
        }

        field_values[i] = llvm::Constant::getNullValue(ctx.llvm_type(struct_type.fields()[i].type));
    }

    return llvm::ConstantStruct::get(llvm_struct_type, field_values);
}

}  // namespace rain::lang::code
