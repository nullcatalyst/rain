#include "rain/code/compiler.hpp"
// ^ Keep at top

#include "rain/ast/expr/all.hpp"
#include "rain/ast/type/all.hpp"
#include "rain/err/simple.hpp"

namespace rain::code {

util::Result<llvm::Value*> Compiler::build(Context&                         ctx,
                                           const ast::IdentifierExpression& identifier_expression) {
    const Variable* const var = ctx.scope.find_variable(identifier_expression.name());
    if (var == nullptr) {
        return ERR_PTR(err::SimpleError,
                       absl::StrFormat("unknown variable: %s", identifier_expression.name()));
    }

    identifier_expression.set_type(var->_type);

    if (var->_alloca) {
        return _llvm_ir.CreateLoad(var->_llvm_type, var->_llvm_value);
    }
    return var->_llvm_value;
}

util::Result<llvm::Value*> Compiler::build(Context&                      ctx,
                                           const ast::IntegerExpression& integer_expression) {
    // TODO: Fix this, don't rely on the name of the type.
    integer_expression.set_type(_builtin_scope.find_named_type("i32"));

    return llvm::ConstantInt::get(*_llvm_ctx, llvm::APInt(32, integer_expression.value()));
}

util::Result<llvm::Value*> Compiler::build(Context&                    ctx,
                                           const ast::FloatExpression& float_expression) {
    // TODO: Fix this, don't rely on the name of the type.
    float_expression.set_type(_builtin_scope.find_named_type("f32"));

    return llvm::ConstantFP::get(*_llvm_ctx, llvm::APFloat(float_expression.value()));
}

util::Result<llvm::Value*> Compiler::build(Context&                     ctx,
                                           const ast::MemberExpression& member_expression) {
    auto owner_expression = build(ctx, member_expression.expression());
    FORWARD_ERROR(owner_expression);

    auto owner_type = member_expression.expression()->type();
    if (owner_type == nullptr) {
        return ERR_PTR(err::SimpleError, "cannot access member of unknown type");
    }
    if (owner_type->kind() == ast::TypeKind::UnresolvedType) {
        owner_type = ctx.scope.resolve_type(owner_type);
    }
    if (owner_type->kind() != ast::TypeKind::StructType) {
        return ERR_PTR(err::SimpleError, "cannot access member of non-struct type");
    }

    const auto llvm_type = ctx.scope.find_llvm_type(owner_type);
    if (llvm_type == nullptr) {
        return ERR_PTR(err::SimpleError, "cannot find llvm type for struct type");
    }

    const auto* const struct_type = static_cast<const ast::StructType*>(owner_type.get());
    const auto        field_index = struct_type->find_field(member_expression.member());
    if (!field_index.has_value()) {
        if (struct_type->is_named()) {
            return ERR_PTR(
                err::SimpleError,
                absl::StrCat("unknown member; \"", member_expression.member(),
                             "\" not found in struct \"", struct_type->name_or_empty(), "\""));
        } else {
            return ERR_PTR(err::SimpleError,
                           absl::StrCat("unknown member; \"", member_expression.member(),
                                        "\" not found in unnamed struct"));
        }
    }

    auto member_type = struct_type->fields()[field_index.value()].type;
    assert(member_type != nullptr && "member type is null");
    auto llvm_member_type = ctx.scope.find_llvm_type(member_type);
    assert(llvm_member_type != nullptr && "cannot find llvm type for member type");
    member_expression.set_type(member_type);

    if (member_expression.expression()->kind() == ast::ExpressionKind::IdentifierExpression) {
        const auto* const identifier_expression =
            static_cast<const ast::IdentifierExpression*>(member_expression.expression().get());
        const auto* const var = ctx.scope.find_variable(identifier_expression->name());
        // This should always be true, since the call to `build()` above must have succeeded to get
        // here.
        assert(var != nullptr && "variable not found");

        if (var->_alloca) {
            return _llvm_ir.CreateLoad(
                llvm_member_type,
                _llvm_ir.CreateStructGEP(var->_llvm_type, var->_llvm_value, field_index.value()));
        }
    }

    return _llvm_ir.CreateExtractValue(std::move(owner_expression).value(), field_index.value());
}

util::Result<llvm::Value*> Compiler::build(Context&                   ctx,
                                           const ast::CallExpression& call_expression) {
    auto callee = build(ctx, call_expression.callee());
    FORWARD_ERROR(callee);

    if (call_expression.callee()->kind() != ast::ExpressionKind::IdentifierExpression) {
        return ERR_PTR(err::SimpleError,
                       "cannot call non-identifier expression: currently not implemented");
    }

    const auto* const identifier_expression =
        static_cast<const ast::IdentifierExpression*>(call_expression.callee().get());
    const auto* const         var = ctx.scope.find_variable(identifier_expression->name());
    llvm::FunctionType* const llvm_function_type = llvm::cast<llvm::FunctionType>(var->_llvm_type);

    llvm::SmallVector<llvm::Value*, 4> llvm_arguments;
    llvm_arguments.reserve(call_expression.arguments().size());
    for (const auto& argument : call_expression.arguments()) {
        auto llvm_argument = build(ctx, argument);
        FORWARD_ERROR(llvm_argument);
        llvm_arguments.emplace_back(std::move(llvm_argument).value());
    }

    return _llvm_ir.CreateCall(llvm_function_type, std::move(callee).value(), llvm_arguments);
}

util::Result<llvm::Value*> Compiler::build(Context&                   ctx,
                                           const ast::CtorExpression& ctor_expression) {
    const auto type = ctx.scope.resolve_type(ctor_expression.type());
    if (type == nullptr) {
        return ERR_PTR(err::SimpleError, "unknown type");
    }

    if (type->kind() != ast::TypeKind::StructType) {
        return ERR_PTR(err::SimpleError, "cannot construct non-struct type");
    }
    const auto* const struct_type = static_cast<const ast::StructType*>(type.get());

    const auto llvm_type = ctx.scope.find_llvm_type(type);
    if (llvm_type == nullptr) {
        return ERR_PTR(err::SimpleError, "cannot find llvm type for struct type");
    }

    // Use insertvalue to set the fields of the struct.
    // We start with either a poison value (if the user explicitly initializes all the fields --
    // this is an optimization) or a zero-initialized value (if one or more fields are not set).
    llvm::Value* llvm_value = ctor_expression.fields().size() == struct_type->fields().size()
                                  ? llvm::PoisonValue::get(llvm_type)
                                  : llvm::Constant::getNullValue(llvm_type);

    for (const auto& field : ctor_expression.fields()) {
        auto llvm_field_value = build(ctx, field.value);
        FORWARD_ERROR(llvm_field_value);

        const auto field_index = struct_type->find_field(field.name);
        if (!field_index.has_value()) {
            if (struct_type->is_named()) {
                return ERR_PTR(err::SimpleError, absl::StrCat("unknown member; \"", field.name,
                                                              "\" not found in struct \"",
                                                              struct_type->name_or_empty(), "\""));
            } else {
                return ERR_PTR(err::SimpleError, absl::StrCat("unknown member; \"", field.name,
                                                              "\" not found in unnamed struct"));
            }
        }

        llvm_value = _llvm_ir.CreateInsertValue(llvm_value, std::move(llvm_field_value).value(),
                                                field_index.value());
    }

    return llvm_value;
}

}  // namespace rain::code
