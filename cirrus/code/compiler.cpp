#include "cirrus/code/compiler.hpp"

#include "absl/strings/str_cat.h"
#include "cirrus/ast/expr/all.hpp"
#include "cirrus/ast/type/all.hpp"
#include "cirrus/code/target.hpp"
#include "cirrus/err/simple.hpp"
#include "llvm/Transforms/Utils/Cloning.h"

namespace llvm {

typedef GenericValue (*ExFunc)(FunctionType*, ArrayRef<GenericValue>);
void InterpreterRegisterExternalFunction(const std::string& name, ExFunc fn);
void InterpreterRemoveExternalFunction(const std::string& name);

}  // namespace llvm

namespace cirrus::code {

namespace {

llvm::ExecutionEngine* create_interpreter(
    llvm::Module* llvm_mod, std::unique_ptr<llvm::TargetMachine> llvm_target_machine) {
    llvm::EngineBuilder builder{std::unique_ptr<llvm::Module>(llvm_mod)};
    std::string         error;
    builder.setErrorStr(&error);
    builder.setEngineKind(llvm::EngineKind::Interpreter);
    auto* engine = builder.create(llvm_target_machine.release());
    if (engine == nullptr) {
        cirrus::util::console_error(ANSI_RED, "failed to create interpreter: ", ANSI_RESET, error);
        std::abort();
    }
    return engine;
}

}  // namespace

void Compiler::use_external_function(const std::string_view function_name,
                                     llvm::GenericValue (*external_function)(
                                         llvm::FunctionType*, llvm::ArrayRef<llvm::GenericValue>)) {
    llvm::InterpreterRegisterExternalFunction(absl::StrCat("lle_X_", function_name),
                                              external_function);
}

Compiler::Compiler()
    : _llvm_ctx(std::make_shared<llvm::LLVMContext>()),
      _llvm_target_machine(wasm_target_machine()),
      _llvm_ir(*_llvm_ctx),
      _builtin_scope(Scope::builtin_scope(*_llvm_ctx)) {
    _llvm_mod    = new llvm::Module("cirrus", *_llvm_ctx);
    _llvm_engine = std::unique_ptr<llvm::ExecutionEngine>(
        create_interpreter(_llvm_mod, wasm_target_machine()));
}

void Compiler::_initialize_builtins() {
    _llvm_mod    = new llvm::Module("cirrus", *_llvm_ctx);
    _llvm_engine = std::unique_ptr<llvm::ExecutionEngine>(
        create_interpreter(_llvm_mod, wasm_target_machine()));
}

util::Result<std::shared_ptr<ast::FunctionType>> Compiler::get_function_type(
    std::vector<ast::TypePtr> argument_types, std::optional<ast::TypePtr> return_type) {
    return ast::FunctionType::alloc(std::move(argument_types), std::move(return_type));
}

util::Result<void> Compiler::declare_external_function(
    const std::string_view name, std::shared_ptr<ast::FunctionType> function_type) {
    if (name.empty()) {
        return ERR_PTR(err::SimpleError, "cannot declare external function, no name given");
    }
    if (function_type == nullptr) {
        return ERR_PTR(err::SimpleError,
                       "cannot declare external function, no function type given");
    }

    Context ctx(*_llvm_mod, *_llvm_engine, _builtin_scope);

    llvm::Type* llvm_return_type = llvm::Type::getVoidTy(*_llvm_ctx);
    if (function_type->return_type().has_value()) {
        auto llvm_found_type = find_or_build_type(ctx, function_type->return_type().value());
        FORWARD_ERROR(llvm_found_type);
        llvm_return_type = std::move(llvm_found_type).value();
        // llvm_return_type =
        //     FORWARD_OR_UNWRAP(find_or_build_type(ctx, function_type->return_type().value()));
    }

    std::vector<llvm::Type*> llvm_argument_types;
    llvm_argument_types.reserve(function_type->argument_types().size());
    for (const auto& argument_type : function_type->argument_types()) {
        auto llvm_argument_type = find_or_build_type(ctx, argument_type);
        FORWARD_ERROR(llvm_argument_type);
        llvm_argument_types.emplace_back(std::move(llvm_argument_type).value());
    }

    llvm::FunctionType* llvm_function_type =
        llvm::FunctionType::get(llvm_return_type, llvm_argument_types, false);

    auto llvm_global         = _llvm_mod->getOrInsertFunction(name, llvm_function_type);
    auto llvm_function_value = llvm::cast<llvm::Function>(llvm_global.getCallee());
    llvm_function_value->setLinkage(llvm::Function::ExternalLinkage);

    _builtin_scope.set_variable(name, Variable{
                                          ._value   = llvm_function_value,
                                          ._type    = llvm_function_type,
                                          ._mutable = false,
                                          ._alloca  = false,
                                      });

    return {};
}

util::Result<Module> Compiler::build(const lang::Module& lang_mod) {
    Scope   scope(&_builtin_scope);
    Context ctx(*_llvm_mod, *_llvm_engine, scope);

    for (const auto& expression : lang_mod.expressions()) {
        switch (expression->kind()) {
            case ast::ExpressionKind::TypeDeclarationExpression: {
                auto llvm_type = build(
                    ctx, *static_cast<const ast::TypeDeclarationExpression*>(expression.get()));
                FORWARD_ERROR(llvm_type);
                break;
            }

            case ast::ExpressionKind::ExportExpression: {
                auto llvm_type =
                    build(ctx, *static_cast<const ast::ExportExpression*>(expression.get()));
                FORWARD_ERROR(llvm_type);
                break;
            }

            case ast::ExpressionKind::FunctionExpression: {
                auto llvm_type =
                    build(ctx, *static_cast<const ast::FunctionExpression*>(expression.get()));
                FORWARD_ERROR(llvm_type);
                break;
            }

            default: {
                cirrus::util::console_error(ANSI_RED, "cannot compile expression: unknown kind",
                                            ANSI_RESET);
                std::abort();
                break;
            }
        }
    }

    auto llvm_mod_clone = llvm::CloneModule(*_llvm_mod);
    llvm_mod_clone->setDataLayout(_llvm_target_machine->createDataLayout());
    llvm_mod_clone->setTargetTriple(_llvm_target_machine->getTargetTriple().str());

    scope.set_parent(nullptr);

    Module code_mod(_llvm_ctx, std::move(llvm_mod_clone), std::move(scope));

    // Reset the compiler.
    _initialize_builtins();

    return std::move(code_mod);
}

util::Result<llvm::Value*> Compiler::build(Context& ctx, const ast::ExpressionPtr& expression) {
    switch (expression->kind()) {
        case ast::ExpressionKind::FunctionExpression:
            return build(ctx, *static_cast<const ast::FunctionExpression*>(expression.get()));

        case ast::ExpressionKind::LetExpression:
            return build(ctx, *static_cast<const ast::LetExpression*>(expression.get()));

        case ast::ExpressionKind::ReturnExpression:
            return build(ctx, *static_cast<const ast::ReturnExpression*>(expression.get()));

        case ast::ExpressionKind::IntegerExpression:
            return build(ctx, *static_cast<const ast::IntegerExpression*>(expression.get()));

        case ast::ExpressionKind::FloatExpression:
            return build(ctx, *static_cast<const ast::FloatExpression*>(expression.get()));

        case ast::ExpressionKind::IdentifierExpression:
            return build(ctx, *static_cast<const ast::IdentifierExpression*>(expression.get()));

        case ast::ExpressionKind::BinaryOperatorExpression:
            return build(ctx, *static_cast<const ast::BinaryOperatorExpression*>(expression.get()));

        case ast::ExpressionKind::CallExpression:
            return build(ctx, *static_cast<const ast::CallExpression*>(expression.get()));

        case ast::ExpressionKind::ExecExpression:
            return build(ctx, *static_cast<const ast::ExecExpression*>(expression.get()));

        case ast::ExpressionKind::BlockExpression:
            return build(ctx, *static_cast<const ast::BlockExpression*>(expression.get()));

        case ast::ExpressionKind::IfExpression:
            return build(ctx, *static_cast<const ast::IfExpression*>(expression.get()));

        default:
            return ERR_PTR(err::SimpleError, "cannot compile expression: unknown expression kind");
    }
}

}  // namespace cirrus::code
