#pragma once

#include <memory>

#include "llvm/ADT/ArrayRef.h"
#include "llvm/ExecutionEngine/ExecutionEngine.h"
#include "llvm/ExecutionEngine/GenericValue.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Type.h"
#include "llvm/Target/TargetMachine.h"
#include "rain/ast/expr/all.hpp"
#include "rain/ast/type/all.hpp"
#include "rain/code/module.hpp"
#include "rain/lang/module.hpp"
#include "rain/util/result.hpp"

namespace rain::code {

class Type;

class Compiler {
    std::shared_ptr<llvm::LLVMContext>   _llvm_ctx;
    std::unique_ptr<llvm::TargetMachine> _llvm_target_machine;
    llvm::Module* _llvm_mod = nullptr;  // This is owned by the ExecutionEngine
    std::unique_ptr<llvm::ExecutionEngine> _llvm_engine;
    llvm::IRBuilder<>                      _llvm_ir;
    Scope                                  _builtin_scope;

  public:
    static void use_external_function(const std::string_view function_name,
                                      llvm::GenericValue (*external_function)(
                                          llvm::FunctionType*, llvm::ArrayRef<llvm::GenericValue>));

    Compiler();
    Compiler(const Compiler&)            = delete;
    Compiler& operator=(const Compiler&) = delete;
    Compiler(Compiler&&)                 = delete;
    Compiler& operator=(Compiler&&)      = delete;
    ~Compiler()                          = default;

    [[nodiscard]] const Scope& builtin_scope() const noexcept { return _builtin_scope; }

    util::Result<std::shared_ptr<ast::FunctionType>> get_function_type(
        std::vector<ast::TypePtr> argument_types, std::optional<ast::TypePtr> return_type);
    util::Result<void> declare_external_function(const std::string_view wasm_namespace,
                                                 const std::string_view wasm_function_name,
                                                 const std::string_view name,
                                                 std::shared_ptr<ast::FunctionType> function_type);

    util::Result<Module> build(const lang::Module& lang_mod);

  private:
    void _initialize_builtins();
    void _add_externref_support();

    struct Context {
        llvm::Module&          llvm_mod;
        llvm::ExecutionEngine& llvm_engine;
        Scope&                 scope;
        bool                   returned = false;

        Context(llvm::Module& llvm_mod, llvm::ExecutionEngine& llvm_engine, Scope& scope)
            : llvm_mod(llvm_mod), llvm_engine(llvm_engine), scope(scope) {}
        Context(const Context& other, Scope& scope)
            : llvm_mod(other.llvm_mod), llvm_engine(other.llvm_engine), scope(scope) {}
    };

    // Types
    util::Result<llvm::Type*> find_or_build_type(Context& ctx, const ast::TypePtr& type);

    util::Result<llvm::StructType*> build(Context& ctx, const ast::StructType& struct_type);
    util::Result<llvm::Type*>       build(
              Context& ctx, const ast::TypeDeclarationExpression& type_declaration_expresion);

    // Expressions
    util::Result<llvm::Value*> build(Context& ctx, const ast::ExpressionPtr& expression);

    // Literals
    util::Result<llvm::Value*> build(Context&                      ctx,
                                     const ast::IntegerExpression& integer_expression);
    util::Result<llvm::Value*> build(Context& ctx, const ast::FloatExpression& float_expression);
    util::Result<llvm::Value*> build(Context&                         ctx,
                                     const ast::IdentifierExpression& identifier_expression);
    util::Result<llvm::Value*> build(Context& ctx, const ast::MemberExpression& member_expression);
    util::Result<llvm::Value*> build(Context& ctx, const ast::CallExpression& call_expression);
    util::Result<llvm::Value*> build(Context& ctx, const ast::CtorExpression& ctor_expression);

    // Operators
    util::Result<llvm::Value*> build(Context& ctx, const ast::ExecExpression& exec_expression);
    util::Result<llvm::Value*> build(Context&                             ctx,
                                     const ast::BinaryOperatorExpression& binop_expression);

    // Statements
    util::Result<void> build(Context& ctx, const ast::StatementExpression& statement_expression);
    util::Result<llvm::Function*> build(Context&                     ctx,
                                        const ast::ExportExpression& export_expression);
    util::Result<llvm::Function*> build(Context&                       ctx,
                                        const ast::FunctionExpression& function_expression);
    util::Result<llvm::Value*>    build(Context& ctx, const ast::LetExpression& let_expression);
    util::Result<llvm::Value*> build(Context& ctx, const ast::ReturnExpression& return_expression);
    util::Result<llvm::Value*> build(Context& ctx, const ast::BlockExpression& block_expression);
    util::Result<llvm::Value*> build(Context& ctx, const ast::IfExpression& if_expression);
};

}  // namespace rain::code
