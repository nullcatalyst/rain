#include "llvm/ExecutionEngine/GenericValue.h"
#include "rain/lang/code/compile/all.hpp"
#include "rain/util/defer.hpp"

namespace rain::lang::code {

namespace {

llvm::Constant* create_constant_from_generic_value(llvm::Type*               llvm_type,
                                                   const llvm::GenericValue& value) {
    switch (llvm_type->getTypeID()) {
        case llvm::Type::IntegerTyID:
            return llvm::ConstantInt::get(llvm_type, value.IntVal);

        case llvm::Type::FloatTyID:
            return llvm::ConstantFP::get(llvm_type, value.FloatVal);

        case llvm::Type::DoubleTyID:
            return llvm::ConstantFP::get(llvm_type, value.DoubleVal);

        case llvm::Type::StructTyID: {
            llvm::StructType* llvm_struct_type = reinterpret_cast<llvm::StructType*>(llvm_type);
            std::vector<llvm::Constant*> struct_constants;
            struct_constants.reserve(value.AggregateVal.size());
            for (int i = 0; i < value.AggregateVal.size(); ++i) {
                struct_constants.push_back(create_constant_from_generic_value(
                    llvm_struct_type->getElementType(i), value.AggregateVal[i]));
            }
            return llvm::ConstantStruct::get(llvm_struct_type, struct_constants);
        }

        default:
            util::panic("unsupported generic value type in constant expression");
    }
}

}  // namespace

llvm::Value* compile_compile_time(Context& ctx, ast::CompileTimeExpression& compile_time) {
    if (!compile_time.compile_time_capable()) {
        // TODO: Emit warning
        util::console_log("WARNING: expression is not compile-time capable");
        return compile_any_expression(ctx, compile_time.expression());
    }

    ast::Type*  type      = compile_time.expression().type();
    llvm::Type* llvm_type = ctx.llvm_type(type);

    llvm::FunctionType* llvm_function_type =
        llvm::FunctionType::get(llvm_type, llvm::ArrayRef<llvm::Type*>(), false);
    llvm::Function* llvm_function = llvm::Function::Create(
        llvm_function_type, llvm::Function::InternalLinkage, "#exec", ctx.llvm_module());

    auto&       llvm_ir                = ctx.llvm_builder();
    auto*       llvm_prev_insert_block = llvm_ir.GetInsertBlock();
    util::Defer reset_ir_position([&]() {
        if (llvm_prev_insert_block != nullptr) {
            llvm_ir.SetInsertPoint(llvm_prev_insert_block);
        }
    });

    llvm::BasicBlock* llvm_entry_block =
        llvm::BasicBlock::Create(ctx.llvm_context(), "entry", llvm_function);
    llvm_ir.SetInsertPoint(llvm_entry_block);

    auto llvm_value = compile_any_expression(ctx, compile_time.expression());
    llvm_ir.CreateRet(llvm_value);

    llvm::GenericValue llvm_return_value =
        ctx.llvm_engine().runFunction(llvm_function, llvm::ArrayRef<llvm::GenericValue>());
    llvm_function->eraseFromParent();

    return create_constant_from_generic_value(llvm_type, llvm_return_value);
}

}  // namespace rain::lang::code
