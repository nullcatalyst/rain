#include "cirrus/code/compiler.hpp"
// ^ Keep at top

#include <llvm/ExecutionEngine/GenericValue.h>
#include <llvm/MC/TargetRegistry.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/Target/TargetMachine.h>
#include <llvm/Transforms/Utils/Cloning.h>

#include "absl/strings/str_format.h"
#include "cirrus/ast/expr/all.hpp"
#include "cirrus/ast/type/all.hpp"
#include "cirrus/err/simple.hpp"
#include "cirrus/util/defer.hpp"

// Always export all functions, even if they are not used.
// TODO: This is temporary, and should be removed. It's here for easier debugging.
#define ALWAYS_EXPORT 0

namespace cirrus::code {

util::Result<llvm::Function*> Compiler::build(Context&                     ctx,
                                              const ast::ExportExpression& export_expression) {
    const ast::FunctionExpression& function_expression =
        *std::static_pointer_cast<ast::FunctionExpression>(export_expression.expression());
    auto llvm_function = build(ctx, function_expression);
    FORWARD_ERROR(llvm_function);

    llvm::Function* llvm_function_value = std::move(llvm_function).value();
    llvm_function_value->setLinkage(llvm::Function::ExternalLinkage);

    {
        // Export the function, making it callable from JS (instead of just WASM).
        llvm::Attribute wasm_export_attr =
            llvm::Attribute::get(llvm_function_value->getContext(), "wasm-export-name",
                                 function_expression.name_or_empty());
        llvm_function_value->addFnAttr(wasm_export_attr);
    }

    return llvm_function_value;
}

util::Result<llvm::Function*> Compiler::build(Context&                       ctx,
                                              const ast::FunctionExpression& function_expression) {
    llvm::Type* llvm_return_type = llvm::Type::getVoidTy(*_llvm_ctx);
    if (function_expression.return_type().has_value()) {
        auto llvm_found_type = find_or_build_type(ctx, function_expression.return_type().value());
        FORWARD_ERROR(llvm_found_type);
        llvm_return_type = std::move(llvm_found_type).value();
    }

    std::vector<llvm::Type*> llvm_argument_types;
    llvm_argument_types.reserve(function_expression.arguments().size());
    for (const auto& argument : function_expression.arguments()) {
        auto llvm_argument_type = find_or_build_type(ctx, argument.type);
        FORWARD_ERROR(llvm_argument_type);
        llvm_argument_types.emplace_back(std::move(llvm_argument_type).value());
    }

    llvm::FunctionType* llvm_function_type =
        llvm::FunctionType::get(llvm_return_type, llvm_argument_types, false);
    llvm::Function* llvm_function = llvm::Function::Create(
        llvm_function_type,
        ALWAYS_EXPORT ? llvm::Function::ExternalLinkage : llvm::Function::InternalLinkage,
        function_expression.name_or_empty(), ctx.llvm_mod);

    if (const auto name = function_expression.name_or_empty(); name.size() > 0) {
        ctx.scope.set_variable(name, Variable{
                                         ._value   = llvm_function,
                                         ._type    = llvm_function_type,
                                         ._mutable = false,
                                         ._alloca  = false,
                                     });
    }

    Scope   function_scope(&ctx.scope);
    Context function_ctx(ctx, function_scope);
    {
        // Set the names of the arguments. This can make the IR more readable
        int argument_index = 0;
        for (const auto& argument : function_expression.arguments()) {
            auto* arg = llvm_function->getArg(argument_index);
            ++argument_index;

            arg->setName(argument.name);
            function_scope.set_variable(argument.name,
                                        Variable{
                                            ._value   = arg,
                                            ._type    = llvm_argument_types[argument_index],
                                            ._mutable = false,
                                            ._alloca  = false,
                                        });
        }
    }

    auto        llvm_prev_insert_block = _llvm_ir.GetInsertBlock();
    auto        llvm_prev_insert_point = _llvm_ir.GetInsertPoint();
    util::Defer reset_ir_position([&]() {
        if (llvm_prev_insert_block != nullptr) {
            _llvm_ir.SetInsertPoint(llvm_prev_insert_block, llvm_prev_insert_point);
        }
    });

    llvm::BasicBlock* llvm_entry_block =
        llvm::BasicBlock::Create(*_llvm_ctx, "entry", llvm_function);
    _llvm_ir.SetInsertPoint(llvm_entry_block);
    for (const auto& expression : function_expression.expressions()) {
        auto llvm_expression_result = build(function_ctx, expression);
        FORWARD_ERROR(llvm_expression_result);
    }

    return llvm_function;
}

util::Result<llvm::Value*> Compiler::build(Context& ctx, const ast::LetExpression& let_expression) {
    auto llvm_let_value = build(ctx, let_expression.value());
    FORWARD_ERROR(llvm_let_value);

    llvm::Value* llvm_value = std::move(llvm_let_value).value();
    if (let_expression.mutable_()) {
        if (llvm_value == nullptr) {
            return ERR_PTR(err::SimpleError, "mutable variable must be initialized with a value");
        }

        // Store the expression value into an alloca.
        llvm::Value* llvm_alloca =
            _llvm_ir.CreateAlloca(llvm_value->getType(), nullptr, let_expression.name());
        _llvm_ir.CreateStore(llvm_value, llvm_alloca);
        llvm_value = llvm_alloca;
    }

    ctx.scope.set_variable(let_expression.name(), Variable{
                                                      ._value   = llvm_value,
                                                      ._type    = nullptr,
                                                      ._mutable = let_expression.mutable_(),
                                                      ._alloca  = let_expression.mutable_(),
                                                  });

    return llvm_value;
}

util::Result<llvm::Value*> Compiler::build(Context&                     ctx,
                                           const ast::ReturnExpression& return_expression) {
    if (return_expression.value().has_value()) {
        auto llvm_return_value = build(ctx, return_expression.value().value());
        FORWARD_ERROR(llvm_return_value);
        llvm::Value* llvm_value = std::move(llvm_return_value).value();
        _llvm_ir.CreateRet(llvm_value);
        ctx.returned = true;
        return llvm_value;
    }

    _llvm_ir.CreateRetVoid();
    ctx.returned = true;
    return nullptr;
}

util::Result<llvm::Value*> Compiler::build(Context&                    ctx,
                                           const ast::BlockExpression& block_expression) {
    Scope   block_scope(ctx.scope);
    Context block_ctx(ctx, block_scope);
    for (const auto& expression : block_expression.expressions()) {
        auto llvm_expression_result = build(block_ctx, expression);
        FORWARD_ERROR(llvm_expression_result);
        if (block_ctx.returned) {
            ctx.returned = true;
            return llvm_expression_result;
        }
    }
    return nullptr;
}

util::Result<llvm::Value*> Compiler::build(Context& ctx, const ast::IfExpression& if_expression) {
    auto llvm_condition = build(ctx, if_expression.condition());
    FORWARD_ERROR(llvm_condition);

    llvm::Function* const   llvm_function = _llvm_ir.GetInsertBlock()->getParent();
    llvm::BasicBlock* const llvm_then_block =
        llvm::BasicBlock::Create(*_llvm_ctx, "then", llvm_function);
    llvm::BasicBlock* const llvm_else_block =
        llvm::BasicBlock::Create(*_llvm_ctx, "else", llvm_function);
    llvm::BasicBlock* const llvm_merge_block =
        llvm::BasicBlock::Create(*_llvm_ctx, "merge", llvm_function);

    _llvm_ir.CreateCondBr(std::move(llvm_condition).value(), llvm_then_block, llvm_else_block);

    _llvm_ir.SetInsertPoint(llvm_then_block);
    auto llvm_then_result = build(ctx, if_expression.then());
    FORWARD_ERROR(llvm_then_result);
    if (!ctx.returned) {
        _llvm_ir.CreateBr(llvm_merge_block);
    }
    const bool then_returned = ctx.returned;
    ctx.returned             = false;

    _llvm_ir.SetInsertPoint(llvm_else_block);
    if (if_expression.else_().has_value()) {
        auto llvm_else_result = build(ctx, if_expression.else_().value());
        FORWARD_ERROR(llvm_else_result);
    }
    if (!ctx.returned) {
        _llvm_ir.CreateBr(llvm_merge_block);
    }

    ctx.returned = then_returned && ctx.returned;

    _llvm_ir.SetInsertPoint(llvm_merge_block);
    return nullptr;
}

}  // namespace cirrus::code
