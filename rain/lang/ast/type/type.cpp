#include "rain/lang/ast/type/type.hpp"

#include "rain/lang/ast/scope/builtin.hpp"
#include "rain/lang/ast/scope/scope.hpp"
#include "rain/lang/ast/var/builtin_function.hpp"
#include "rain/lang/code/context.hpp"
#include "rain/lang/code/type/all.hpp"
#include "rain/lang/serial/operator_names.hpp"

namespace rain::lang::ast {

Type& Type::unwrap(Type& type) noexcept {
    auto* unwrapped = &type;
    for (;;) {
        assert(unwrapped != nullptr && "unwrapped type is null");

        switch (unwrapped->kind()) {
            case serial::TypeKind::Array:
                unwrapped = &static_cast<ArrayType&>(*unwrapped).type();
                break;

            case serial::TypeKind::Optional:
                unwrapped = &static_cast<OptionalType&>(*unwrapped).type();
                break;

            case serial::TypeKind::Reference:
                unwrapped = &static_cast<ReferenceType&>(*unwrapped).type();
                break;

            default:
                return *unwrapped;
        }
    }
}

OptionalType& Type::get_optional_type(Scope& scope) {
    if (_optional_type != nullptr) {
        return *_optional_type;
    }
    _optional_type      = std::make_unique<OptionalType>(this);
    auto* optional_type = _optional_type.get();
    if (_resolves_to != this) {
        return *optional_type;
    }

    ast::Type* optional_reference_type = nullptr;
    if (optional_type->type().kind() == serial::TypeKind::Reference) {
        optional_reference_type = optional_type;
    } else {
        optional_reference_type = &optional_type->get_reference_type(scope);
    }

    {      // Add builtin functions for optional types.
        {  // Null check.
            auto* bool_type = scope.builtin()->bool_type();

            auto  unop_args = Scope::TypeList{optional_reference_type};
            auto* function_type =
                scope.get_resolved_function_type(optional_type, unop_args, bool_type);
            auto method = make_builtin_function_variable(
                serial::OperatorNames::HasValue, function_type,
                [optional_type](auto& ctx, auto& arguments) {
                    auto& llvm_ir = ctx.llvm_builder();

                    llvm::Type* llvm_type = arguments[0]->getType();
                    if (optional_type->kind() == serial::TypeKind::Reference) {
                        // If the type is a pointer, then a null check is sufficient.
                        return llvm_ir.CreateICmpNE(arguments[0],
                                                    llvm::Constant::getNullValue(llvm_type));
                    }

                    // Otherwise load the tag and compare it to null.
                    auto* llvm_i1_type      = llvm::Type::getInt1Ty(ctx.llvm_context());
                    auto* llvm_optional_ref = get_or_compile_type(ctx, *optional_type);
                    return llvm_ir.CreateICmpNE(
                        llvm_ir.CreateLoad(llvm_i1_type,
                                           llvm_ir.CreateConstInBoundsGEP2_32(llvm_optional_ref,
                                                                              arguments[0], 0, 1)),
                        llvm::Constant::getNullValue(llvm_i1_type));
                });
            scope.add_resolved_function(std::move(method));
        }
    }

    return *optional_type;
}

ReferenceType& Type::get_reference_type(Scope& scope) {
    if (_reference_type != nullptr) {
        return *_reference_type;
    }
    _reference_type = std::make_unique<ReferenceType>(this);
    return *_reference_type;
}

ArrayType& Type::get_array_type(Scope& scope, size_t length) {
    auto it = _array_types.find(length);
    if (it != _array_types.end()) {
        return *it->second;
    }

    auto* array_type =
        _array_types.emplace(length, std::make_unique<ArrayType>(this, length)).first->second.get();
    if (_resolves_to != this) {
        return *array_type;
    }

    {      // Add builtin functions for array types.
        {  // Array indexing.
            auto* index_type     = scope.builtin()->i32_type();
            auto* reference_type = &get_reference_type(scope);

            auto* function_type = scope.get_resolved_function_type(
                array_type, {array_type, index_type}, reference_type);
            auto method = make_builtin_function_variable(
                serial::OperatorNames::ArrayIndex, function_type,
                [array_type](auto& ctx, auto& arguments) {
                    auto& llvm_ir         = ctx.llvm_builder();
                    auto* llvm_array_type = ctx.llvm_type(array_type);
                    assert(llvm_array_type != nullptr && "llvm array type is null");
                    auto* llvm_element_type = ctx.llvm_type(&array_type->type());
                    assert(llvm_element_type != nullptr && "llvm element type is null");
                    return llvm_ir.CreateGEP(llvm_array_type, arguments[0],
                                             {llvm_ir.getInt32(0), arguments[1]});
                });
            scope.add_resolved_function(std::move(method));
        }

        {  // Array length.
            auto* index_type = scope.builtin()->i32_type();

            auto* function_type = scope.get_resolved_function_type(array_type, {}, index_type);
            auto  method        = make_builtin_function_variable(
                "length", function_type, [array_type](auto& ctx, auto& arguments) {
                    return llvm::ConstantInt::get(llvm::Type::getInt32Ty(ctx.llvm_context()),
                                                          static_cast<uint64_t>(array_type->length()));
                });
            scope.add_resolved_function(std::move(method));
        }
    }

    return *array_type;
}

util::Result<absl::Nonnull<Type*>> Type::resolve(Options& options, Scope& scope) {
    if (_resolves_to != nullptr) {
        return _resolves_to;
    }

    auto result = _resolve(options, scope);
    FORWARD_ERROR(result);

    _resolves_to = std::move(result).value();
    return _resolves_to;
}

////////////////////////////////////////////////////////////////
// ArrayType

ArrayType::ArrayType(absl::Nonnull<Type*> type, size_t length) : _type(type), _length(length) {}

ArrayType::ArrayType(absl::Nonnull<Type*> type, size_t length, lex::Location location)
    : _type(type), _length(length), _location(location) {}

std::string ArrayType::display_name() const noexcept {
    return absl::StrCat("[", _length, "]", _type->display_name());
}

util::Result<absl::Nonnull<Type*>> ArrayType::_resolve(Options& options, Scope& scope) {
    {
        auto result = _type->resolve(options, scope);
        FORWARD_ERROR(result);

        _type = std::move(result).value();
    }

    return &_type->get_array_type(scope, _length);
}

////////////////////////////////////////////////////////////////
// OptionalType

OptionalType::OptionalType(absl::Nonnull<Type*> type) : _type(type) {}

OptionalType::OptionalType(absl::Nonnull<Type*> type, lex::Location location)
    : _type(std::move(type)), _location(location) {}

std::string OptionalType::display_name() const noexcept {
    return absl::StrCat("?", _type->display_name());
}

util::Result<absl::Nonnull<Type*>> OptionalType::_resolve(Options& options, Scope& scope) {
    {
        auto result = _type->resolve(options, scope);
        FORWARD_ERROR(result);

        _type = std::move(result).value();
    }

    return &_type->get_optional_type(scope);
}

////////////////////////////////////////////////////////////////
// ReferenceType

ReferenceType::ReferenceType(absl::Nonnull<Type*> type) : _type(type) {}

ReferenceType::ReferenceType(absl::Nonnull<Type*> type, lex::Location location)
    : _type(std::move(type)), _location(location) {}

std::string ReferenceType::display_name() const noexcept {
    return absl::StrCat("&", _type->display_name());
}

util::Result<absl::Nonnull<Type*>> ReferenceType::_resolve(Options& options, Scope& scope) {
    {
        auto result = _type->resolve(options, scope);
        FORWARD_ERROR(result);

        _type = std::move(result).value();
    }

    return &_type->get_reference_type(scope);
}

}  // namespace rain::lang::ast
