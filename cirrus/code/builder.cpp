#include "cirrus/code/builder.hpp"

#include <llvm/ExecutionEngine/GenericValue.h>
#include <llvm/MC/TargetRegistry.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/Target/TargetMachine.h>
#include <llvm/Transforms/Utils/Cloning.h>

#include "absl/strings/str_format.h"
#include "cirrus/ast/expr/all.hpp"
#include "cirrus/ast/type/all.hpp"
#include "cirrus/err/simple.hpp"
#include "cirrus/util/before_return.hpp"

// Always export all functions, even if they are not used.
// TODO: This is temporary, and should be removed. It's here for easier debugging.
#define ALWAYS_EXPORT 0

namespace cirrus::code {

namespace {

// TODO: remove this
// This is a temporary way to determine whether a block (like an if statement) has been returned
// from, so that we can avoid creating branches after returns.
bool _returned = false;

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

void Builder::initialize_llvm() {
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

Builder::Builder()
    : _llvm_ctx(std::make_shared<llvm::LLVMContext>()),
      _llvm_target_machine(get_wasm_target_machine()),
      _llvm_ir(*_llvm_ctx),
      _builtin_scope(Scope::builtin_scope(*_llvm_ctx)) {}

util::Result<Module> Builder::build(const lang::Module& mod) {
    llvm::Module*                          llvm_mod    = new llvm::Module("cirrus", *_llvm_ctx);
    std::unique_ptr<llvm::ExecutionEngine> llvm_engine = std::unique_ptr<llvm::ExecutionEngine>(
        create_interpreter(llvm_mod, get_wasm_target_machine()));

    Scope   scope(&_builtin_scope);
    Context ctx(*llvm_mod, *llvm_engine, scope);

    for (const auto& node : mod.nodes()) {
        switch (node.kind()) {
            case ast::NodeKind::StructType: {
                auto llvm_struct_type = build(ctx, ast::StructType::from(node));
                FORWARD_ERROR_WITH_TYPE(Module, llvm_struct_type);
                break;
            }

            case ast::NodeKind::ExportExpression: {
                auto llvm_function_type = build(ctx, ast::ExportExpression::from(node));
                FORWARD_ERROR_WITH_TYPE(Module, llvm_function_type);
                break;
            }

            default: {
                auto llvm_value = build(ctx, ast::Expression::from(node));
            }
        }
    }

    auto llvm_mod_clone = llvm::CloneModule(*llvm_mod);
    llvm_mod_clone->setDataLayout(_llvm_target_machine->createDataLayout());
    llvm_mod_clone->setTargetTriple(_llvm_target_machine->getTargetTriple().str());
    // delete llvm_engine;

    scope.set_parent(nullptr);
    return OK(Module,
              Module(_llvm_ctx, _llvm_target_machine, std::move(llvm_mod_clone), std::move(scope)));
}

util::Result<llvm::Type*> Builder::find_or_build_type(const Context& ctx, const ast::Type& type) {
    switch (type.kind()) {
        case ast::NodeKind::StructType: {
            const auto struct_type = ast::StructType::from(type);
            if (const auto name = struct_type.name_or_empty(); name.empty()) {
                auto llvm_struct_type = build(ctx, struct_type);
                FORWARD_ERROR_WITH_TYPE(llvm::Type*, llvm_struct_type);
                return OK(llvm::Type*, llvm_struct_type.unwrap());
            } else {
                if (llvm::Type* const llvm_type = ctx.scope.find_llvm_type(name);
                    llvm_type != nullptr) {
                    return OK(llvm::Type*, llvm_type);
                } else {
                    auto llvm_struct_type = build(ctx, struct_type);
                    FORWARD_ERROR_WITH_TYPE(llvm::Type*, llvm_struct_type);
                    ctx.scope.set_llvm_type(name, llvm_struct_type.unwrap());
                    return OK(llvm::Type*, llvm_struct_type.unwrap());
                }
            }
        }

        case ast::NodeKind::UnresolvedType: {
            const auto unresolved_type = ast::UnresolvedType::from(type);
            if (llvm::Type* const llvm_type = ctx.scope.find_llvm_type(unresolved_type.name());
                llvm_type == nullptr) {
                return ERR_PTR(llvm::Type*, err::SimpleError,
                               absl::StrFormat("unknown type: %s", unresolved_type.name()));
            } else {
                return OK(llvm::Type*, llvm_type);
            }
        }

        default:
            return ERR_PTR(llvm::Type*, err::SimpleError, "unknown type");
    }
}

util::Result<llvm::StructType*> Builder::build(const Context&         ctx,
                                               const ast::StructType& struct_type) {
    auto llvm_struct_type = llvm::StructType::create(*_llvm_ctx, struct_type.name_or_empty());
    std::vector<llvm::Type*> llvm_field_types;
    for (const auto& field : struct_type.fields()) {
        auto llvm_argument_type_result = find_or_build_type(ctx, field.type);
        FORWARD_ERROR_WITH_TYPE(llvm::StructType*, llvm_argument_type_result);
        llvm_field_types.emplace_back(llvm_argument_type_result.unwrap());
    }
    llvm_struct_type->setBody(llvm_field_types);
    return OK(llvm::StructType*, llvm_struct_type);
}

util::Result<llvm::Value*> Builder::build(const Context& ctx, const ast::Expression& expression) {
    switch (expression.kind()) {
        case ast::NodeKind::FunctionExpression:
            return build(ctx, ast::FunctionExpression::from(expression));

        case ast::NodeKind::ReturnExpression:
            return build(ctx, ast::ReturnExpression::from(expression));

        case ast::NodeKind::IntegerExpression:
            return build(ctx, ast::IntegerExpression::from(expression));

        case ast::NodeKind::IdentifierExpression:
            return build(ctx, ast::IdentifierExpression::from(expression));

        case ast::NodeKind::BinaryOperatorExpression:
            return build(ctx, ast::BinaryOperatorExpression::from(expression));

        case ast::NodeKind::CallExpression:
            return build(ctx, ast::CallExpression::from(expression));

        case ast::NodeKind::ExecExpression:
            return build(ctx, ast::ExecExpression::from(expression));

        case ast::NodeKind::BlockExpression:
            return build(ctx, ast::BlockExpression::from(expression));

        case ast::NodeKind::IfExpression:
            return build(ctx, ast::IfExpression::from(expression));

        default:
            return ERR_PTR(llvm::Value*, err::SimpleError, "not implemented");
    }
}

util::Result<llvm::Function*> Builder::build(const Context&               ctx,
                                             const ast::ExportExpression& export_expression) {
    auto function_expression  = ast::FunctionExpression::from(export_expression.expression());
    auto llvm_function_result = build(ctx, function_expression);
    FORWARD_ERROR_WITH_TYPE(llvm::Function*, llvm_function_result);

    auto llvm_function = llvm_function_result.unwrap();
    {
        // Export the function, making it callable from JS (instead of just WASM).
        llvm::Attribute wasm_export_attr = llvm::Attribute::get(
            llvm_function->getContext(), "wasm-export-name", function_expression.name_or_empty());
        llvm_function->addFnAttr(wasm_export_attr);

        llvm_function->setLinkage(llvm::Function::ExternalLinkage);
    }

    return OK(llvm::Function*, llvm_function);
}

util::Result<llvm::Function*> Builder::build(const Context&                 ctx,
                                             const ast::FunctionExpression& function_expression) {
    llvm::Type* llvm_return_type = llvm::Type::getVoidTy(*_llvm_ctx);
    if (function_expression.return_type().has_value()) {
        auto llvm_return_type_result =
            find_or_build_type(ctx, function_expression.return_type().value());
        FORWARD_ERROR_WITH_TYPE(llvm::Function*, llvm_return_type_result);
        llvm_return_type = llvm_return_type_result.unwrap();
    }

    std::vector<llvm::Type*> llvm_argument_types;
    llvm_argument_types.reserve(function_expression.arguments().size());
    for (const auto& argument : function_expression.arguments()) {
        auto llvm_argument_type_result = find_or_build_type(ctx, argument.type);
        FORWARD_ERROR_WITH_TYPE(llvm::Function*, llvm_argument_type_result);
        llvm_argument_types.emplace_back(llvm_argument_type_result.unwrap());
    }

    llvm::FunctionType* llvm_function_type =
        llvm::FunctionType::get(llvm_return_type, llvm_argument_types, false);
    llvm::Function* llvm_function = llvm::Function::Create(
        llvm_function_type,
        ALWAYS_EXPORT ? llvm::Function::ExternalLinkage : llvm::Function::InternalLinkage,
        function_expression.name_or_empty(), ctx.llvm_mod);

    ctx.scope.set_variable(function_expression.name_or_empty(), Variable{
                                                                    ._value   = llvm_function,
                                                                    ._mutable = false,
                                                                    ._alloca  = false,
                                                                });

    Scope   function_scope(&ctx.scope);
    Context function_ctx(ctx, function_scope);
    {
        // Set the names of the arguments. This can make the IR more readable
        int argument_index = 0;
        for (const auto& argument : function_expression.arguments()) {
            auto* arg = llvm_function->getArg(argument_index);
            ++argument_index;

            arg->setName(argument.name);
            function_scope.set_variable(argument.name, Variable{
                                                           ._value   = arg,
                                                           ._mutable = false,
                                                       });
        }
    }

    auto               llvm_prev_insert_block = _llvm_ir.GetInsertBlock();
    auto               llvm_prev_insert_point = _llvm_ir.GetInsertPoint();
    util::BeforeReturn reset_ir_position([&]() {
        if (llvm_prev_insert_block != nullptr) {
            _llvm_ir.SetInsertPoint(llvm_prev_insert_block, llvm_prev_insert_point);
        }
    });

    llvm::BasicBlock* llvm_entry_block =
        llvm::BasicBlock::Create(*_llvm_ctx, "entry", llvm_function);
    _llvm_ir.SetInsertPoint(llvm_entry_block);
    for (const auto& expression : function_expression.expressions()) {
        auto llvm_expression_result = build(function_ctx, expression);
        FORWARD_ERROR_WITH_TYPE(llvm::Function*, llvm_expression_result);
    }

    return OK(llvm::Function*, llvm_function);
}

util::Result<llvm::Value*> Builder::build(const Context&               ctx,
                                          const ast::ReturnExpression& return_expression) {
    _returned = true;

    if (return_expression.expr().has_value()) {
        auto llvm_return_value_result = build(ctx, return_expression.expr().value());
        FORWARD_ERROR_WITH_TYPE(llvm::Value*, llvm_return_value_result);
        _llvm_ir.CreateRet(llvm_return_value_result.unwrap());
        return OK(llvm::Value*, llvm_return_value_result.unwrap());
    }

    _llvm_ir.CreateRetVoid();
    return OK(llvm::Value*, nullptr);
}

util::Result<llvm::Value*> Builder::build(const Context&                   ctx,
                                          const ast::IdentifierExpression& identifier_expression) {
    const Variable* const var = ctx.scope.find_variable(identifier_expression.name());
    if (var == nullptr) {
        return ERR_PTR(llvm::Value*, err::SimpleError,
                       absl::StrFormat("unknown variable: %s", identifier_expression.name()));
    }

    return OK(llvm::Value*, var->_value);
}

util::Result<llvm::Value*> Builder::build(const Context&                ctx,
                                          const ast::IntegerExpression& integer_expression) {
    return OK(llvm::Value*,
              llvm::ConstantInt::get(*_llvm_ctx, llvm::APInt(32, integer_expression.value())));
}

util::Result<llvm::Value*> Builder::build(const Context&             ctx,
                                          const ast::CallExpression& call_expression) {
    // TODO: Get the proper function type instead of assuming all `i32`s.
    llvm::Type* const                 llvm_i32_type = llvm::Type::getInt32Ty(*_llvm_ctx);
    llvm::SmallVector<llvm::Type*, 4> llvm_argument_types;
    llvm_argument_types.reserve(call_expression.arguments().size());
    for (const auto& argument : call_expression.arguments()) {
        llvm_argument_types.emplace_back(llvm_i32_type);
    }
    llvm::FunctionType* const llvm_function_type =
        llvm::FunctionType::get(llvm_i32_type, llvm_argument_types, false);

    auto callee = build(ctx, call_expression.callee());
    FORWARD_ERROR_WITH_TYPE(llvm::Value*, callee);

    llvm::SmallVector<llvm::Value*, 4> llvm_arguments;
    llvm_arguments.reserve(call_expression.arguments().size());
    for (const auto& argument : call_expression.arguments()) {
        auto llvm_argument_result = build(ctx, argument);
        FORWARD_ERROR_WITH_TYPE(llvm::Value*, llvm_argument_result);
        llvm_arguments.emplace_back(llvm_argument_result.unwrap());
    }

    return OK(llvm::Value*,
              _llvm_ir.CreateCall(llvm_function_type, callee.unwrap(), llvm_arguments));
}

util::Result<llvm::Value*> Builder::build(const Context&                       ctx,
                                          const ast::BinaryOperatorExpression& binop_expression) {
    auto llvm_lhs_result = build(ctx, binop_expression.lhs());
    FORWARD_ERROR_WITH_TYPE(llvm::Value*, llvm_lhs_result);
    auto llvm_rhs_result = build(ctx, binop_expression.rhs());
    FORWARD_ERROR_WITH_TYPE(llvm::Value*, llvm_rhs_result);

    switch (binop_expression.op()) {
        case ast::BinaryOperator::Add:
            return OK(llvm::Value*,
                      _llvm_ir.CreateAdd(llvm_lhs_result.unwrap(), llvm_rhs_result.unwrap()));
        case ast::BinaryOperator::Subtract:
            return OK(llvm::Value*,
                      _llvm_ir.CreateSub(llvm_lhs_result.unwrap(), llvm_rhs_result.unwrap()));
        case ast::BinaryOperator::Multiply:
            return OK(llvm::Value*,
                      _llvm_ir.CreateMul(llvm_lhs_result.unwrap(), llvm_rhs_result.unwrap()));
        case ast::BinaryOperator::Divide:
            return OK(llvm::Value*,
                      _llvm_ir.CreateSDiv(llvm_lhs_result.unwrap(), llvm_rhs_result.unwrap()));
        case ast::BinaryOperator::Modulo:
            return OK(llvm::Value*,
                      _llvm_ir.CreateSRem(llvm_lhs_result.unwrap(), llvm_rhs_result.unwrap()));
        case ast::BinaryOperator::Equal:
            return OK(llvm::Value*,
                      _llvm_ir.CreateICmpEQ(llvm_lhs_result.unwrap(), llvm_rhs_result.unwrap()));
        case ast::BinaryOperator::NotEqual:
            return OK(llvm::Value*,
                      _llvm_ir.CreateICmpNE(llvm_lhs_result.unwrap(), llvm_rhs_result.unwrap()));
        case ast::BinaryOperator::Less:
            return OK(llvm::Value*,
                      _llvm_ir.CreateICmpSLT(llvm_lhs_result.unwrap(), llvm_rhs_result.unwrap()));
        case ast::BinaryOperator::LessEqual:
            return OK(llvm::Value*,
                      _llvm_ir.CreateICmpSLE(llvm_lhs_result.unwrap(), llvm_rhs_result.unwrap()));
        case ast::BinaryOperator::Greater:
            return OK(llvm::Value*,
                      _llvm_ir.CreateICmpSGT(llvm_lhs_result.unwrap(), llvm_rhs_result.unwrap()));
        case ast::BinaryOperator::GreaterEqual:
            return OK(llvm::Value*,
                      _llvm_ir.CreateICmpSGE(llvm_lhs_result.unwrap(), llvm_rhs_result.unwrap()));
        default:
            return ERR_PTR(llvm::Value*, err::SimpleError, "not implemented");
    }
}

util::Result<llvm::Value*> Builder::build(const Context&             ctx,
                                          const ast::ExecExpression& exec_expression) {
    // TODO: Determine return type from expression.
    llvm::Type*         llvm_return_type = llvm::Type::getInt32Ty(*_llvm_ctx);
    llvm::FunctionType* llvm_function_type =
        llvm::FunctionType::get(llvm_return_type, llvm::ArrayRef<llvm::Type*>(), false);
    llvm::Function* llvm_function = llvm::Function::Create(
        llvm_function_type, llvm::Function::InternalLinkage, "#exec", ctx.llvm_mod);

    auto               llvm_prev_insert_block = _llvm_ir.GetInsertBlock();
    auto               llvm_prev_insert_point = _llvm_ir.GetInsertPoint();
    util::BeforeReturn reset_ir_position([&]() {
        if (llvm_prev_insert_block != nullptr) {
            _llvm_ir.SetInsertPoint(llvm_prev_insert_block, llvm_prev_insert_point);
        }
    });

    llvm::BasicBlock* llvm_entry_block =
        llvm::BasicBlock::Create(*_llvm_ctx, "entry", llvm_function);
    _llvm_ir.SetInsertPoint(llvm_entry_block);
    auto llvm_value = build(ctx, exec_expression.expression());
    FORWARD_ERROR_WITH_TYPE(llvm::Value*, llvm_value);
    _llvm_ir.CreateRet(llvm_value.unwrap());

    llvm::GenericValue llvm_return_value =
        ctx.llvm_engine.runFunction(llvm_function, llvm::ArrayRef<llvm::GenericValue>());

    llvm_function->eraseFromParent();

    return OK(llvm::Value*,
              llvm::ConstantInt::get(*_llvm_ctx,
                                     llvm::APInt(32, llvm_return_value.IntVal.getSExtValue())));
}

util::Result<llvm::Value*> Builder::build(const Context&              ctx,
                                          const ast::BlockExpression& block_expression) {
    Scope   block_scope(ctx.scope);
    Context block_ctx(ctx, block_scope);
    for (const auto& expression : block_expression.expressions()) {
        auto llvm_expression_result = build(block_ctx, expression);
        FORWARD_ERROR_WITH_TYPE(llvm::Value*, llvm_expression_result);
    }
    return OK(llvm::Value*, nullptr);
}

util::Result<llvm::Value*> Builder::build(const Context&           ctx,
                                          const ast::IfExpression& if_expression) {
    auto llvm_condition_result = build(ctx, if_expression.condition());
    FORWARD_ERROR_WITH_TYPE(llvm::Value*, llvm_condition_result);

    llvm::Function* const   llvm_function = _llvm_ir.GetInsertBlock()->getParent();
    llvm::BasicBlock* const llvm_then_block =
        llvm::BasicBlock::Create(*_llvm_ctx, "then", llvm_function);
    llvm::BasicBlock* const llvm_else_block =
        llvm::BasicBlock::Create(*_llvm_ctx, "else", llvm_function);
    llvm::BasicBlock* const llvm_merge_block =
        llvm::BasicBlock::Create(*_llvm_ctx, "merge", llvm_function);

    _llvm_ir.CreateCondBr(llvm_condition_result.unwrap(), llvm_then_block, llvm_else_block);

    _llvm_ir.SetInsertPoint(llvm_then_block);
    auto llvm_then_result = build(ctx, if_expression.then());
    FORWARD_ERROR_WITH_TYPE(llvm::Value*, llvm_then_result);
    if (!_returned) {
        _llvm_ir.CreateBr(llvm_merge_block);
    }
    _returned = false;

    _llvm_ir.SetInsertPoint(llvm_else_block);
    if (if_expression.else_().has_value()) {
        auto llvm_else_result = build(ctx, if_expression.else_().value());
        FORWARD_ERROR_WITH_TYPE(llvm::Value*, llvm_else_result);
    }
    if (!_returned) {
        _llvm_ir.CreateBr(llvm_merge_block);
    }
    _returned = false;

    _llvm_ir.SetInsertPoint(llvm_merge_block);
    return OK(llvm::Value*, nullptr);
}

}  // namespace cirrus::code
