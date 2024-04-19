#include "rain/lang/ast/expr/compile_time.hpp"

#include "llvm/ExecutionEngine/GenericValue.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/IntrinsicInst.h"
#include "llvm/IR/Type.h"
#include "rain/lang/code/context.hpp"
#include "rain/lang/code/expr/any.hpp"
#include "rain/lang/code/type/all.hpp"
#include "rain/util/defer.hpp"

namespace rain::lang::code {

namespace {

llvm::Constant* create_constant(const llvm::DataLayout& data_layout, llvm::Type* const llvm_type,
                                const llvm::TypeSize type_size, const void* const data) {
    switch (llvm_type->getTypeID()) {
        case llvm::Type::IntegerTyID: {
            switch (static_cast<uint64_t>(type_size)) {
                case 1:
                    return llvm::ConstantInt::get(
                        llvm_type, llvm::APInt(8, *static_cast<const uint8_t*>(data)));
                case 2:
                    return llvm::ConstantInt::get(
                        llvm_type, llvm::APInt(16, *static_cast<const uint16_t*>(data)));
                case 4:
                    return llvm::ConstantInt::get(
                        llvm_type, llvm::APInt(32, *static_cast<const uint32_t*>(data)));
                case 8:
                    return llvm::ConstantInt::get(
                        llvm_type, llvm::APInt(64, *static_cast<const uint64_t*>(data)));
                default:
                    util::panic("unsupported integer size in constant expression");
            }
        }

        case llvm::Type::FloatTyID:
            return llvm::ConstantFP::get(llvm_type, *static_cast<const float*>(data));

        case llvm::Type::DoubleTyID:
            return llvm::ConstantFP::get(llvm_type, *static_cast<const double*>(data));

        case llvm::Type::FixedVectorTyID: {
            auto* const llvm_vector_type = llvm::cast<llvm::FixedVectorType>(llvm_type);
            const auto  element_size =
                data_layout.getTypeAllocSize(llvm_vector_type->getElementType());
            std::vector<llvm::Constant*> vector_constants;
            vector_constants.reserve(llvm_vector_type->getNumElements());
            for (int i = 0; i < llvm_vector_type->getNumElements(); ++i) {
                vector_constants.push_back(
                    create_constant(data_layout, llvm_vector_type->getElementType(), element_size,
                                    static_cast<const uint8_t*>(data) + i * element_size));
            }
            return llvm::ConstantVector::get(vector_constants);
        }

        case llvm::Type::ArrayTyID: {
            auto* const llvm_array_type = llvm::cast<llvm::ArrayType>(llvm_type);
            const auto  element_size =
                data_layout.getTypeAllocSize(llvm_array_type->getElementType());

            std::vector<llvm::Constant*> array_constants;
            array_constants.reserve(llvm_array_type->getNumElements());
            for (int i = 0; i < llvm_array_type->getNumElements(); ++i) {
                array_constants.push_back(
                    create_constant(data_layout, llvm_array_type->getElementType(), element_size,
                                    static_cast<const uint8_t*>(data) + i * element_size));
            }
            return llvm::ConstantArray::get(llvm_array_type, array_constants);
        }

        case llvm::Type::StructTyID: {
            auto* const                  llvm_struct_type = llvm::cast<llvm::StructType>(llvm_type);
            std::vector<llvm::Constant*> struct_constants;
            struct_constants.reserve(llvm_struct_type->getNumElements());
            for (int i = 0; i < llvm_struct_type->getNumElements(); ++i) {
                const auto element_size =
                    data_layout.getTypeAllocSize(llvm_struct_type->getElementType(i));
                struct_constants.push_back(
                    create_constant(data_layout, llvm_struct_type->getElementType(i), element_size,
                                    static_cast<const uint8_t*>(data) + element_size));
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
        // TODO: Emit warning instead of error.
        // Unfortunately, this more than one diagnostic being emitted is not yet supported.
        util::console_log("WARNING: expression is not compile-time capable");
        return compile_any_expression(ctx, compile_time.expression());
    }

    ast::Type* type = compile_time.expression().type();
    assert(type != nullptr && "compile-time expression has no type");
    llvm::Type* llvm_type = get_or_compile_type(ctx, *type);
    assert(llvm_type != nullptr && "failed to get llvm type for compile-time expression");

    llvm::FunctionType* llvm_function_type = llvm::FunctionType::get(
        llvm::Type::getVoidTy(ctx.llvm_context()), llvm_type->getPointerTo(), false);
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

    auto&       llvm_engine  = ctx.llvm_engine();
    const auto& data_layout  = llvm_engine.getDataLayout();
    const auto  size_of_type = data_layout.getTypeAllocSize(llvm_type);

    // Allocate memory for the output value. The pointer will be passed in as the sole argument, the
    // compiled function will write the value to the memory location.
    void* const ptr = malloc(size_of_type);
    util::Defer free_ptr([&]() { free(ptr); });
    memset(ptr, 0, size_of_type);

    // It appears that the LLVM interpreter does not support assigning compound types (SIMD types
    // are treated as a primitive). So instead we have to manually assign each field.
    if (llvm_type->isArrayTy()) {
        auto* llvm_alloca = llvm_ir.CreateAlloca(llvm_type);

        for (int i = 0; i < llvm_type->getArrayNumElements(); ++i) {
            std::array<llvm::Value*, 2> indices{
                llvm::ConstantInt::get(ctx.llvm_context(), llvm::APInt(32, 0)),
                llvm::ConstantInt::get(ctx.llvm_context(), llvm::APInt(32, i)),
            };
            llvm_ir.CreateStore(llvm_ir.CreateExtractValue(llvm_value, {static_cast<unsigned>(i)}),
                                llvm_ir.CreateInBoundsGEP(llvm_type, llvm_alloca, indices));
        }

        llvm_ir.CreateMemCpy(llvm_function->arg_begin(), llvm::MaybeAlign(), llvm_alloca,
                             llvm::MaybeAlign(), size_of_type);
    } else if (llvm_type->isStructTy()) {
        auto* llvm_alloca = llvm_ir.CreateAlloca(llvm_type);

        for (int i = 0; i < llvm_type->getStructNumElements(); ++i) {
            llvm_ir.CreateStore(llvm_ir.CreateExtractValue(llvm_value, {static_cast<unsigned>(i)}),
                                llvm_ir.CreateStructGEP(llvm_type, llvm_alloca, i));
        }

        llvm_ir.CreateMemCpy(llvm_function->arg_begin(), llvm::MaybeAlign(), llvm_alloca,
                             llvm::MaybeAlign(), size_of_type);
    } else {
        // Primitive type.
        llvm_ir.CreateStore(llvm_value, llvm_function->arg_begin());
    }
    llvm_ir.CreateRetVoid();

    llvm::GenericValue llvm_generic_ptr;
    llvm_generic_ptr.PointerVal = ptr;
    ctx.llvm_engine().runFunction(llvm_function, llvm_generic_ptr);
    llvm_function->eraseFromParent();

    return create_constant(data_layout, llvm_type, data_layout.getTypeAllocSize(llvm_type), ptr);
}

}  // namespace rain::lang::code
