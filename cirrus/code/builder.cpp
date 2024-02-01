#include "cirrus/code/builder.hpp"

#include <llvm/Support/TargetSelect.h>

#include "absl/strings/str_format.h"
#include "cirrus/ast/expr/all.hpp"
#include "cirrus/ast/type/all.hpp"
#include "cirrus/err/simple.hpp"
#include "cirrus/util/before_return.hpp"

// Always export all functions, even if they are not used.
// TODO: This is temporary, and should be removed. It's here for easier debugging.
#define ALWAYS_EXPORT 0

namespace cirrus::code {

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
    }
}

util::Result<Module> Builder::build(const lang::Module& mod) {
    Module code_mod(_ctx);
    code_mod.exported_scope().set_parent(&_builtin_scope);
    util::BeforeReturn before_return(
        [&code_mod]() { code_mod.exported_scope().set_parent(nullptr); });
    Scope& scope = code_mod.exported_scope();

    for (const auto& node : mod.nodes()) {
        switch (node.kind()) {
            case ast::NodeKind::StructType: {
                auto llvm_struct_type = build(code_mod, scope, ast::StructType::from(node));
                FORWARD_ERROR_WITH_TYPE(Module, llvm_struct_type);
                break;
            }

            case ast::NodeKind::ExportExpression: {
                auto llvm_function_type = build(code_mod, scope, ast::ExportExpression::from(node));
                FORWARD_ERROR_WITH_TYPE(Module, llvm_function_type);
                break;
            }

            default: {
                auto llvm_value = build(code_mod, scope, ast::Expression::from(node));
            }
        }
    }

    return OK(Module, std::move(code_mod));
}

util::Result<llvm::Type*> Builder::find_or_build_type(Module& code_mod, Scope& scope,
                                                      const ast::Type& type) {
    switch (type.kind()) {
        case ast::NodeKind::StructType: {
            const auto struct_type = ast::StructType::from(type);
            if (const auto name = struct_type.name_or_empty(); name.empty()) {
                auto llvm_struct_type = build(code_mod, scope, struct_type);
                FORWARD_ERROR_WITH_TYPE(llvm::Type*, llvm_struct_type);
                return OK(llvm::Type*, llvm_struct_type.unwrap());
            } else {
                if (llvm::Type* const llvm_type = scope.find_llvm_type(name);
                    llvm_type != nullptr) {
                    return OK(llvm::Type*, llvm_type);
                } else {
                    auto llvm_struct_type = build(code_mod, scope, struct_type);
                    FORWARD_ERROR_WITH_TYPE(llvm::Type*, llvm_struct_type);
                    scope.set_llvm_type(name, llvm_struct_type.unwrap());
                    return OK(llvm::Type*, llvm_struct_type.unwrap());
                }
            }
        }

        case ast::NodeKind::UnresolvedType: {
            const auto unresolved_type = ast::UnresolvedType::from(type);
            if (llvm::Type* const llvm_type = scope.find_llvm_type(unresolved_type.name());
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

util::Result<llvm::StructType*> Builder::build(Module& code_mod, Scope& scope,
                                               const ast::StructType& struct_type) {
    auto llvm_struct_type = llvm::StructType::create(*_ctx, struct_type.name_or_empty());
    std::vector<llvm::Type*> llvm_field_types;
    for (const auto& field : struct_type.fields()) {
        auto llvm_argument_type_result = find_or_build_type(code_mod, scope, field.type);
        FORWARD_ERROR_WITH_TYPE(llvm::StructType*, llvm_argument_type_result);
        llvm_field_types.emplace_back(llvm_argument_type_result.unwrap());
    }
    llvm_struct_type->setBody(llvm_field_types);
    return OK(llvm::StructType*, llvm_struct_type);
}

util::Result<llvm::Value*> Builder::build(Module& code_mod, Scope& scope,
                                          const ast::Expression& expression) {
    switch (expression.kind()) {
        case ast::NodeKind::FunctionExpression:
            return build(code_mod, scope, ast::FunctionExpression::from(expression));

        case ast::NodeKind::ReturnExpression:
            return build(code_mod, scope, ast::ReturnExpression::from(expression));

        case ast::NodeKind::IntegerExpression:
            return build(code_mod, scope, ast::IntegerExpression::from(expression));

        case ast::NodeKind::IdentifierExpression:
            return build(code_mod, scope, ast::IdentifierExpression::from(expression));

        case ast::NodeKind::BinaryOperatorExpression:
            return build(code_mod, scope, ast::BinaryOperatorExpression::from(expression));

        case ast::NodeKind::CallExpression:
            return build(code_mod, scope, ast::CallExpression::from(expression));

        default:
            return ERR_PTR(llvm::Value*, err::SimpleError, "not implemented");
    }
}

util::Result<llvm::Function*> Builder::build(Module& code_mod, Scope& scope,
                                             const ast::ExportExpression& export_expression) {
    auto function_expression  = ast::FunctionExpression::from(export_expression.expression());
    auto llvm_function_result = build(code_mod, scope, function_expression);
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

util::Result<llvm::Function*> Builder::build(Module& code_mod, Scope& scope,
                                             const ast::FunctionExpression& function_expression) {
    llvm::Type* llvm_return_type = llvm::Type::getVoidTy(*_ctx);
    if (function_expression.return_type().has_value()) {
        auto llvm_return_type_result =
            find_or_build_type(code_mod, scope, function_expression.return_type().value());
        FORWARD_ERROR_WITH_TYPE(llvm::Function*, llvm_return_type_result);
        llvm_return_type = llvm_return_type_result.unwrap();
    }

    std::vector<llvm::Type*> llvm_argument_types;
    llvm_argument_types.reserve(function_expression.arguments().size());
    for (const auto& argument : function_expression.arguments()) {
        auto llvm_argument_type_result = find_or_build_type(code_mod, scope, argument.type);
        FORWARD_ERROR_WITH_TYPE(llvm::Function*, llvm_argument_type_result);
        llvm_argument_types.emplace_back(llvm_argument_type_result.unwrap());
    }

    llvm::FunctionType* llvm_function_type =
        llvm::FunctionType::get(llvm_return_type, llvm_argument_types, false);
    llvm::Function* llvm_function = llvm::Function::Create(
        llvm_function_type,
        ALWAYS_EXPORT ? llvm::Function::ExternalLinkage : llvm::Function::InternalLinkage,
        function_expression.name_or_empty(), code_mod.llvm_module());

    Scope function_scope(&scope);

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

    auto               llvm_prev_insert_block = _ir.GetInsertBlock();
    auto               llvm_prev_insert_point = _ir.GetInsertPoint();
    util::BeforeReturn reset_ir_position([&]() {
        if (llvm_prev_insert_block != nullptr) {
            _ir.SetInsertPoint(llvm_prev_insert_block, llvm_prev_insert_point);
        }
    });

    llvm::BasicBlock* llvm_entry_block = llvm::BasicBlock::Create(*_ctx, "entry", llvm_function);
    _ir.SetInsertPoint(llvm_entry_block);
    for (const auto& expression : function_expression.expressions()) {
        auto llvm_expression_result = build(code_mod, function_scope, expression);
        FORWARD_ERROR_WITH_TYPE(llvm::Function*, llvm_expression_result);
    }

    scope.set_variable(function_expression.name_or_empty(), Variable{
                                                                ._value   = llvm_function,
                                                                ._mutable = false,
                                                                ._alloca  = false,
                                                            });
    return OK(llvm::Function*, llvm_function);
}

util::Result<llvm::Value*> Builder::build(Module& code_mod, Scope& scope,
                                          const ast::ReturnExpression& return_expression) {
    if (return_expression.expr().has_value()) {
        auto llvm_return_value_result = build(code_mod, scope, return_expression.expr().value());
        FORWARD_ERROR_WITH_TYPE(llvm::Value*, llvm_return_value_result);
        _ir.CreateRet(llvm_return_value_result.unwrap());
        return OK(llvm::Value*, llvm_return_value_result.unwrap());
    }

    _ir.CreateRetVoid();
    return OK(llvm::Value*, nullptr);
}

util::Result<llvm::Value*> Builder::build(Module& code_mod, Scope& scope,
                                          const ast::IdentifierExpression& identifier_expression) {
    const Variable* const var = scope.find_variable(identifier_expression.name());
    if (var == nullptr) {
        return ERR_PTR(llvm::Value*, err::SimpleError,
                       absl::StrFormat("unknown variable: %s", identifier_expression.name()));
    }

    return OK(llvm::Value*, var->_value);
}

util::Result<llvm::Value*> Builder::build(Module& code_mod, Scope& scope,
                                          const ast::IntegerExpression& integer_expression) {
    return OK(llvm::Value*,
              llvm::ConstantInt::get(*_ctx, llvm::APInt(32, integer_expression.value())));
}

util::Result<llvm::Value*> Builder::build(Module& code_mod, Scope& scope,
                                          const ast::CallExpression& call_expression) {
    // TODO: Get the proper function type instead of assuming all `i32`s.
    llvm::Type* const                 llvm_i32_type = llvm::Type::getInt32Ty(*_ctx);
    llvm::SmallVector<llvm::Type*, 4> llvm_argument_types;
    llvm_argument_types.reserve(call_expression.arguments().size());
    for (const auto& argument : call_expression.arguments()) {
        llvm_argument_types.emplace_back(llvm_i32_type);
    }
    llvm::FunctionType* const llvm_function_type =
        llvm::FunctionType::get(llvm_i32_type, llvm_argument_types, false);

    auto callee = build(code_mod, scope, call_expression.callee());
    FORWARD_ERROR_WITH_TYPE(llvm::Value*, callee);

    llvm::SmallVector<llvm::Value*, 4> llvm_arguments;
    llvm_arguments.reserve(call_expression.arguments().size());
    for (const auto& argument : call_expression.arguments()) {
        auto llvm_argument_result = build(code_mod, scope, argument);
        FORWARD_ERROR_WITH_TYPE(llvm::Value*, llvm_argument_result);
        llvm_arguments.emplace_back(llvm_argument_result.unwrap());
    }

    return OK(llvm::Value*, _ir.CreateCall(llvm_function_type, callee.unwrap(), llvm_arguments));
}

util::Result<llvm::Value*> Builder::build(Module& code_mod, Scope& scope,
                                          const ast::BinaryOperatorExpression& binop_expression) {
    auto llvm_lhs_result = build(code_mod, scope, binop_expression.lhs());
    FORWARD_ERROR_WITH_TYPE(llvm::Value*, llvm_lhs_result);
    auto llvm_rhs_result = build(code_mod, scope, binop_expression.rhs());
    FORWARD_ERROR_WITH_TYPE(llvm::Value*, llvm_rhs_result);

    switch (binop_expression.op()) {
        case ast::BinaryOperator::Add:
            return OK(llvm::Value*,
                      _ir.CreateAdd(llvm_lhs_result.unwrap(), llvm_rhs_result.unwrap()));
        case ast::BinaryOperator::Subtract:
            return OK(llvm::Value*,
                      _ir.CreateSub(llvm_lhs_result.unwrap(), llvm_rhs_result.unwrap()));
        case ast::BinaryOperator::Multiply:
            return OK(llvm::Value*,
                      _ir.CreateMul(llvm_lhs_result.unwrap(), llvm_rhs_result.unwrap()));
        case ast::BinaryOperator::Divide:
            return OK(llvm::Value*,
                      _ir.CreateSDiv(llvm_lhs_result.unwrap(), llvm_rhs_result.unwrap()));
        case ast::BinaryOperator::Modulo:
            return OK(llvm::Value*,
                      _ir.CreateSRem(llvm_lhs_result.unwrap(), llvm_rhs_result.unwrap()));
        case ast::BinaryOperator::Equal:
            return OK(llvm::Value*,
                      _ir.CreateICmpEQ(llvm_lhs_result.unwrap(), llvm_rhs_result.unwrap()));
        case ast::BinaryOperator::NotEqual:
            return OK(llvm::Value*,
                      _ir.CreateICmpNE(llvm_lhs_result.unwrap(), llvm_rhs_result.unwrap()));
        case ast::BinaryOperator::Less:
            return OK(llvm::Value*,
                      _ir.CreateICmpSLT(llvm_lhs_result.unwrap(), llvm_rhs_result.unwrap()));
        case ast::BinaryOperator::LessEqual:
            return OK(llvm::Value*,
                      _ir.CreateICmpSLE(llvm_lhs_result.unwrap(), llvm_rhs_result.unwrap()));
        case ast::BinaryOperator::Greater:
            return OK(llvm::Value*,
                      _ir.CreateICmpSGT(llvm_lhs_result.unwrap(), llvm_rhs_result.unwrap()));
        case ast::BinaryOperator::GreaterEqual:
            return OK(llvm::Value*,
                      _ir.CreateICmpSGE(llvm_lhs_result.unwrap(), llvm_rhs_result.unwrap()));
        default:
            return ERR_PTR(llvm::Value*, err::SimpleError, "not implemented");
    }
}

}  // namespace cirrus::code
