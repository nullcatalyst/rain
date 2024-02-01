#pragma once

#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>

#include "cirrus/ast/expr/all.hpp"
#include "cirrus/ast/type/all.hpp"
#include "cirrus/code/module.hpp"
#include "cirrus/lang/module.hpp"
#include "cirrus/util/result.hpp"

namespace cirrus::code {

class Builder {
    std::shared_ptr<llvm::LLVMContext> _ctx;
    llvm::IRBuilder<>                  _ir;
    Scope                              _builtin_scope;

  public:
    static void initialize_llvm();

    Builder()
        : _ctx(std::make_shared<llvm::LLVMContext>()),
          _ir(*_ctx),
          _builtin_scope(Scope::builtin_scope(*_ctx)) {}
    ~Builder() = default;

    util::Result<Module> build(const lang::Module& lang_mod);

  private:
    util::Result<llvm::Type*>       find_or_build_type(Module& code_mod, Scope& scope,
                                                       const ast::Type& type);
    util::Result<llvm::StructType*> build(Module& code_mod, Scope& scope,
                                          const ast::StructType& struct_type);

    util::Result<llvm::Value*> build(Module& code_mod, Scope& scope,
                                     const ast::Expression& expression);

    util::Result<llvm::Function*> build(Module& code_mod, Scope& scope,
                                        const ast::ExportExpression& export_expression);
    util::Result<llvm::Function*> build(Module& code_mod, Scope& scope,
                                        const ast::FunctionExpression& function_expression);
    util::Result<llvm::Value*>    build(Module& code_mod, Scope& scope,
                                        const ast::ReturnExpression& return_expression);
    util::Result<llvm::Value*>    build(Module& code_mod, Scope& scope,
                                        const ast::IntegerExpression& integer_expression);
    util::Result<llvm::Value*>    build(Module& code_mod, Scope& scope,
                                        const ast::IdentifierExpression& identifier_expression);
    util::Result<llvm::Value*>    build(Module& code_mod, Scope& scope,
                                        const ast::CallExpression& call_expression);
    util::Result<llvm::Value*>    build(Module& code_mod, Scope& scope,
                                        const ast::BinaryOperatorExpression& binop_expression);
};

}  // namespace cirrus::code
