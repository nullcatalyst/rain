#include "cirrus/transform/codegen.hpp"

#include "cirrus/ast/expr/all.hpp"
#include "cirrus/ast/type/all.hpp"
#include "cirrus/err/simple.hpp"

namespace cirrus::transform {

Codegen::Codegen() : _ctx(), _mod("cirrus", _ctx), _builder(_ctx) {
    // _mod.setDataLayout(_ctx.getTargetTriple().getDataLayoutStr());

    _types.emplace("i8", llvm::Type::getInt8Ty(_ctx));
    _types.emplace("i16", llvm::Type::getInt16Ty(_ctx));
    _types.emplace("i32", llvm::Type::getInt32Ty(_ctx));
    _types.emplace("i64", llvm::Type::getInt64Ty(_ctx));
    _types.emplace("u8", llvm::Type::getInt8Ty(_ctx));
    _types.emplace("u16", llvm::Type::getInt16Ty(_ctx));
    _types.emplace("u32", llvm::Type::getInt32Ty(_ctx));
    _types.emplace("u64", llvm::Type::getInt64Ty(_ctx));
    _types.emplace("f32", llvm::Type::getFloatTy(_ctx));
    _types.emplace("f64", llvm::Type::getDoubleTy(_ctx));
}

util::Result<void> Codegen::generate(const lang::Module& mod) {
    for (const auto& node : mod.nodes()) {
        switch (node.kind()) {
            case ast::NodeKind::StructType: {
                auto struct_type             = ast::StructType::from(node);
                auto llvm_struct_type_result = generate(struct_type);
                FORWARD_ERROR(llvm_struct_type_result);
            }

            case ast::NodeKind::FunctionExpression: {
                auto function             = ast::FunctionExpression::from(node);
                auto llvm_function_result = generate(function);
                FORWARD_ERROR(llvm_function_result);
                break;
            }

            default:
                return util::Result<std::string>::error(
                    std::make_unique<err::SimpleError>("not implemented"));
        }
    }

    return util::Result<void>::ok();
}

std::string Codegen::llvm_ir() const {
    std::string              str;
    llvm::raw_string_ostream os(str);
    _mod.print(os, nullptr);
    return os.str();
}

util::Result<llvm::Type*> Codegen::find_llvm_type(const ast::Type& type) {
    switch (type.kind()) {
        case ast::NodeKind::StructType: {
            const auto struct_type = ast::StructType::from(type);
            if (const auto name = struct_type.name_or_empty(); name.empty()) {
                auto llvm_struct_type = generate(struct_type);
                FORWARD_ERROR_WITH_TYPE(llvm::Type*, llvm_struct_type);
                return OK(llvm::Type*, llvm_struct_type.unwrap());
            } else {
                if (const auto it = _types.find(name); it == _types.end()) {
                    auto llvm_struct_type = generate(struct_type);
                    FORWARD_ERROR_WITH_TYPE(llvm::Type*, llvm_struct_type);
                    _types.emplace(name, llvm_struct_type.unwrap());
                    return OK(llvm::Type*, llvm_struct_type.unwrap());
                } else {
                    return OK(llvm::Type*, it->second);
                }
            }
        }

        case ast::NodeKind::UnresolvedType: {
            const auto unresolved_type = ast::UnresolvedType::from(type);
            if (const auto it = _types.find(unresolved_type.name()); it == _types.end()) {
                return ERR_PTR(llvm::Type*, err::SimpleError, "unknown type");
            } else {
                return OK(llvm::Type*, it->second);
            }
        }

        default:
            return ERR_PTR(llvm::Type*, err::SimpleError, "unknown type");
    }
}

util::Result<llvm::Function*> Codegen::generate(
    const ast::FunctionExpression& function_expression) {
    llvm::Type* llvm_return_type = llvm::Type::getVoidTy(_ctx);
    if (function_expression.return_type().has_value()) {
        auto llvm_return_type_result = find_llvm_type(function_expression.return_type().value());
        FORWARD_ERROR_WITH_TYPE(llvm::Function*, llvm_return_type_result);
        llvm_return_type = llvm_return_type_result.unwrap();
    }

    std::vector<llvm::Type*> llvm_argument_types;
    llvm_argument_types.reserve(function_expression.arguments().size());
    for (const auto& argument : function_expression.arguments()) {
        auto llvm_argument_type_result = find_llvm_type(argument.type);
        FORWARD_ERROR_WITH_TYPE(llvm::Function*, llvm_argument_type_result);
        llvm_argument_types.emplace_back(llvm_argument_type_result.unwrap());
    }

    llvm::FunctionType* llvm_function_type =
        llvm::FunctionType::get(llvm_return_type, llvm_argument_types, false);
    auto llvm_function = llvm::Function::Create(llvm_function_type, llvm::Function::ExternalLinkage,
                                                function_expression.name_or_empty(), _mod);

    {
        // Set the names of the arguments. This can make the IR more readable
        int argument_index = 0;
        for (const auto& argument : function_expression.arguments()) {
            llvm_function->getArg(argument_index)->setName(argument.name);
            ++argument_index;
        }
    }

    llvm::BasicBlock* llvm_entry_block = llvm::BasicBlock::Create(_ctx, "entry", llvm_function);
    _builder.SetInsertPoint(llvm_entry_block);
    llvm::Value* llvm_return_value = llvm::ConstantInt::get(llvm_return_type, 0);
    _builder.CreateRet(llvm_return_value);

    return OK(llvm::Function*, llvm_function);
}
util::Result<llvm::StructType*> Codegen::generate(const ast::StructType& struct_type) {
    auto llvm_struct_type = llvm::StructType::create(_ctx, struct_type.name_or_empty());
    std::vector<llvm::Type*> llvm_field_types;
    for (const auto& field : struct_type.fields()) {
        llvm_field_types.push_back(llvm::Type::getInt32Ty(_ctx));
    }
    llvm_struct_type->setBody(llvm_field_types);
    return OK(llvm::StructType*, llvm_struct_type);
}

}  // namespace cirrus::transform
