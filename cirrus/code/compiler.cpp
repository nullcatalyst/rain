#include "cirrus/code/compiler.hpp"

#include <llvm/ExecutionEngine/GenericValue.h>
#include <llvm/MC/TargetRegistry.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/Target/TargetMachine.h>
#include <llvm/Transforms/Utils/Cloning.h>

#include "absl/strings/str_format.h"
#include "cirrus/ast/expr/all.hpp"
#include "cirrus/ast/type/all.hpp"
#include "cirrus/err/simple.hpp"

namespace cirrus::code {

namespace {

llvm::TargetMachine* get_wasm_target_machine() {
    std::string error;

    std::string         target_triple = llvm::Triple::normalize("wasm32-unknown-unknown");
    const llvm::Target* target        = llvm::TargetRegistry::lookupTarget(target_triple, error);
    if (target == nullptr) {
        std::cerr << "failed to lookup target: " << error << "\n";
        std::abort();
    }

    std::string cpu;
    std::string features =
        "+simd128,+sign-ext,+bulk-memory,+mutable-globals,+nontrapping-fptoint,+multivalue";
    return target->createTargetMachine(target_triple, cpu, features, llvm::TargetOptions(),
                                       std::nullopt, std::nullopt, llvm::CodeGenOptLevel::Default);
}

llvm::ExecutionEngine* create_interpreter(llvm::Module*        llvm_mod,
                                          llvm::TargetMachine* llvm_target_machine) {
    llvm::EngineBuilder builder{std::unique_ptr<llvm::Module>(llvm_mod)};
    builder.setEngineKind(llvm::EngineKind::Interpreter);
    auto* engine = builder.create(llvm_target_machine);
    if (engine == nullptr) {
        std::cerr << "failed to create interpreter\n";
        std::abort();
    }
    return engine;
}

}  // namespace

void Compiler::initialize_llvm() {
    static bool initialized = false;
    if (!initialized) {
        initialized = true;
        // llvm::InitializeNativeTarget();
        // llvm::InitializeNativeTargetAsmPrinter();
        // llvm::InitializeNativeTargetAsmParser();

        // llvm::InitializeAllTargets();
        // llvm::InitializeAllAsmPrinters();
        // llvm::InitializeAllAsmParsers();

        LLVMInitializeWebAssemblyTargetInfo();
        LLVMInitializeWebAssemblyTarget();
        LLVMInitializeWebAssemblyTargetMC();
        LLVMInitializeWebAssemblyAsmPrinter();
        LLVMInitializeWebAssemblyAsmParser();
        LLVMLinkInInterpreter();
    }
}

Compiler::Compiler()
    : _llvm_ctx(std::make_shared<llvm::LLVMContext>()),
      _llvm_target_machine(get_wasm_target_machine()),
      _llvm_ir(*_llvm_ctx),
      _builtin_scope(Scope::builtin_scope(*_llvm_ctx)) {}

util::Result<Module> Compiler::build(const lang::Module& mod) {
    llvm::Module*                          llvm_mod    = new llvm::Module("cirrus", *_llvm_ctx);
    std::unique_ptr<llvm::ExecutionEngine> llvm_engine = std::unique_ptr<llvm::ExecutionEngine>(
        create_interpreter(llvm_mod, get_wasm_target_machine()));

    Scope   scope(&_builtin_scope);
    Context ctx(*llvm_mod, *llvm_engine, scope);

    for (const auto& node : mod.nodes()) {
        switch (node->kind()) {
            case ast::NodeKind::StructType: {
                auto llvm_struct_type =
                    build(ctx, *std::static_pointer_cast<ast::StructType>(node));
                FORWARD_ERROR(llvm_struct_type);
                break;
            }

            case ast::NodeKind::ExportExpression: {
                auto llvm_function_type =
                    build(ctx, *std::static_pointer_cast<ast::ExportExpression>(node));
                FORWARD_ERROR(llvm_function_type);
                break;
            }

            default: {
                auto llvm_value = build(ctx, std::static_pointer_cast<ast::Expression>(node));
            }
        }
    }

    auto llvm_mod_clone = llvm::CloneModule(*llvm_mod);
    llvm_mod_clone->setDataLayout(_llvm_target_machine->createDataLayout());
    llvm_mod_clone->setTargetTriple(_llvm_target_machine->getTargetTriple().str());

    scope.set_parent(nullptr);
    return Module(_llvm_ctx, _llvm_target_machine, std::move(llvm_mod_clone), std::move(scope));
}

util::Result<llvm::Value*> Compiler::build(Context& ctx, const ast::ExpressionPtr& expression) {
    switch (expression->kind()) {
        case ast::NodeKind::FunctionExpression:
            return build(ctx, *std::static_pointer_cast<ast::FunctionExpression>(expression));

        case ast::NodeKind::LetExpression:
            return build(ctx, *std::static_pointer_cast<ast::LetExpression>(expression));

        case ast::NodeKind::ReturnExpression:
            return build(ctx, *std::static_pointer_cast<ast::ReturnExpression>(expression));

        case ast::NodeKind::IntegerExpression:
            return build(ctx, *std::static_pointer_cast<ast::IntegerExpression>(expression));

        case ast::NodeKind::IdentifierExpression:
            return build(ctx, *std::static_pointer_cast<ast::IdentifierExpression>(expression));

        case ast::NodeKind::BinaryOperatorExpression:
            return build(ctx, *std::static_pointer_cast<ast::BinaryOperatorExpression>(expression));

        case ast::NodeKind::CallExpression:
            return build(ctx, *std::static_pointer_cast<ast::CallExpression>(expression));

        case ast::NodeKind::ExecExpression:
            return build(ctx, *std::static_pointer_cast<ast::ExecExpression>(expression));

        case ast::NodeKind::BlockExpression:
            return build(ctx, *std::static_pointer_cast<ast::BlockExpression>(expression));

        case ast::NodeKind::IfExpression:
            return build(ctx, *std::static_pointer_cast<ast::IfExpression>(expression));

        default:
            return ERR_PTR(err::SimpleError, "not implemented");
    }
}

}  // namespace cirrus::code
