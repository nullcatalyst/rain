#pragma once

#include <llvm/ExecutionEngine/ExecutionEngine.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>

#include <memory>

#include "cirrus/ast/expr/all.hpp"
#include "cirrus/ast/type/all.hpp"
#include "cirrus/code/module.hpp"
#include "cirrus/lang/module.hpp"
#include "cirrus/util/result.hpp"

namespace cirrus::code {

class Builder {
    std::shared_ptr<llvm::LLVMContext>   _llvm_ctx;
    std::shared_ptr<llvm::TargetMachine> _llvm_target_machine;
    llvm::IRBuilder<>                    _llvm_ir;
    Scope                                _builtin_scope;

  public:
    static void initialize_llvm();

    Builder();
    ~Builder() = default;

    util::Result<Module> build(const lang::Module& lang_mod);

  private:
    struct Context {
        llvm::Module&          llvm_mod;
        llvm::ExecutionEngine& llvm_engine;
        Scope&                 scope;

        Context(llvm::Module& llvm_mod, llvm::ExecutionEngine& llvm_engine, Scope& scope)
            : llvm_mod(llvm_mod), llvm_engine(llvm_engine), scope(scope) {}
        Context(const Context& other, Scope& scope)
            : llvm_mod(other.llvm_mod), llvm_engine(other.llvm_engine), scope(scope) {}
    };

    util::Result<llvm::Type*>       find_or_build_type(const Context& ctx, const ast::Type& type);
    util::Result<llvm::StructType*> build(const Context& ctx, const ast::StructType& struct_type);

    util::Result<llvm::Value*> build(const Context& ctx, const ast::Expression& expression);

    util::Result<llvm::Function*> build(const Context&               ctx,
                                        const ast::ExportExpression& export_expression);
    util::Result<llvm::Function*> build(const Context&                 ctx,
                                        const ast::FunctionExpression& function_expression);
    util::Result<llvm::Value*>    build(const Context&               ctx,
                                        const ast::ReturnExpression& return_expression);
    util::Result<llvm::Value*>    build(const Context&                ctx,
                                        const ast::IntegerExpression& integer_expression);
    util::Result<llvm::Value*>    build(const Context&                   ctx,
                                        const ast::IdentifierExpression& identifier_expression);
    util::Result<llvm::Value*>    build(const Context&             ctx,
                                        const ast::CallExpression& call_expression);
    util::Result<llvm::Value*>    build(const Context&                       ctx,
                                        const ast::BinaryOperatorExpression& binop_expression);
    util::Result<llvm::Value*>    build(const Context&             ctx,
                                        const ast::ExecExpression& exec_expression);
    util::Result<llvm::Value*>    build(const Context&              ctx,
                                        const ast::BlockExpression& block_expression);
    util::Result<llvm::Value*>    build(const Context& ctx, const ast::IfExpression& if_expression);
};

}  // namespace cirrus::code
