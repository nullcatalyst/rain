#include "llvm/IR/Type.h"
#include "rain/lang/ast/expr/array_literal.hpp"
#include "rain/lang/ast/expr/slice_literal.hpp"
#include "rain/lang/code/expr/any.hpp"
#include "rain/lang/code/type/all.hpp"

namespace rain::lang::code {

namespace {

llvm::Value* _compile_array_literal(Context& ctx, llvm::ArrayType* llvm_type,
                                    const std::string_view constant_name,
                                    const std::vector<std::unique_ptr<ast::Expression>>& elements) {
    std::vector<llvm::Value*> llvm_element_values;
    llvm_element_values.resize(elements.size(), nullptr);

    // Compile the field values in the order that they are defined.
    // The values will be assigned to the struct in field order, but compiling them in the order
    // that they were written in the source code allows calling functions that have order-dependent
    // side-effects inline in the struct literal.
    for (int i = 0, end = llvm_element_values.size(); i < end; ++i) {
        llvm_element_values[i] = compile_any_expression(ctx, *elements[i]);
    }

    // Check if all fields are constant.
    // This is an optimization, as LLVM allows us to create a constant struct if all fields are also
    // constants. Otherwise, we have to create a struct at runtime with non-constant values.
    const bool is_constant =
        std::all_of(elements.begin(), elements.end(),
                    [](const auto& expression) { return expression->is_constant(); });
    if (is_constant) {
        llvm::ArrayRef<llvm::Constant*> llvm_constants(
            reinterpret_cast<llvm::Constant**>(llvm_element_values.data()),
            llvm_element_values.size());
        llvm::GlobalVariable* llvm_global = new llvm::GlobalVariable(
            ctx.llvm_module(), llvm_type, true, llvm::GlobalValue::InternalLinkage,
            llvm::ConstantArray::get(llvm_type, llvm_constants), constant_name);
        return llvm_global;
    }

    auto& llvm_ir = ctx.llvm_builder();

    // Create a runtime array.
    llvm::Value* llvm_alloca = llvm_ir.CreateAlloca(llvm_type, nullptr, constant_name);
    for (int i = 0, end = llvm_element_values.size(); i < end; ++i) {
        std::array<llvm::Value*, 2> indices{
            llvm::ConstantInt::get(ctx.llvm_context(), llvm::APInt(32, 0)),
            llvm::ConstantInt::get(ctx.llvm_context(), llvm::APInt(32, i)),
        };
        llvm_ir.CreateStore(llvm_element_values[i],
                            llvm_ir.CreateInBoundsGEP(llvm_type, llvm_alloca, indices));
    }
    return llvm_alloca;
}

}  // namespace

llvm::Value* compile_array_literal(Context& ctx, ast::ArrayLiteralExpression& array_literal) {
    ast::ArrayType&  array_type = static_cast<ast::ArrayType&>(*array_literal.type());
    llvm::ArrayType* llvm_type =
        static_cast<llvm::ArrayType*>(get_or_compile_type(ctx, array_type));

    return _compile_array_literal(ctx, llvm_type, absl::StrCat("const.", array_type.display_name()),
                                  array_literal.elements());
}

llvm::Value* compile_slice_literal(Context& ctx, ast::SliceLiteralExpression& slice_literal) {
    ast::SliceType& slice_type        = static_cast<ast::SliceType&>(*slice_literal.type());
    llvm::Type*     llvm_slice_type   = get_or_compile_type(ctx, slice_type);
    llvm::Type*     llvm_element_type = get_or_compile_type(ctx, slice_type.type());

    const auto       element_count   = slice_literal.elements().size();
    llvm::ArrayType* llvm_array_type = llvm::ArrayType::get(llvm_element_type, element_count);

    auto* llvm_array_literal = _compile_array_literal(
        ctx, llvm_array_type, absl::StrCat("const.", slice_type.display_name()),
        slice_literal.elements());

    auto& llvm_ir      = ctx.llvm_builder();
    auto* llvm_poison  = llvm::PoisonValue::get(llvm_slice_type);
    auto* llvm_partial = llvm_ir.CreateInsertValue(
        llvm_poison, llvm_ir.CreateConstGEP2_32(llvm_array_type, llvm_array_literal, 0, 0), 0);
    return llvm_ir.CreateInsertValue(
        llvm_partial,
        llvm_ir.CreateConstGEP2_32(llvm_array_type, llvm_array_literal, 0, element_count), 1);
}

}  // namespace rain::lang::code
