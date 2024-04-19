#include "rain/lang/ast/type/type.hpp"

#include "rain/lang/ast/scope/builtin.hpp"
#include "rain/lang/ast/scope/scope.hpp"
#include "rain/lang/ast/var/builtin_function.hpp"
#include "rain/lang/code/context.hpp"

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
    _optional_type = std::make_unique<OptionalType>(this);
    return *_optional_type;
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

    {
        // Add builtin functions for array types.

        {  // Array indexing.
            auto* index_type     = scope.builtin()->i32_type();
            auto* reference_type = &get_reference_type(scope);

            auto  binop_args = Scope::TypeList{array_type, index_type};
            auto* function_type =
                scope.get_resolved_function_type(array_type, binop_args, reference_type);
            auto method = make_builtin_function_variable(
                "__getitem__", function_type, [](auto& ctx, auto& arguments) {
                    return ctx.llvm_builder().CreateGEP(arguments[0]->getType(), arguments[0],
                                                        {arguments[1]});
                });
            scope.add_resolved_function(std::move(method));
        }

        {  // Array length.
            auto* index_type = scope.builtin()->i32_type();

            auto  binop_args = Scope::TypeList{};
            auto* function_type =
                scope.get_resolved_function_type(array_type, binop_args, index_type);
            auto method = make_builtin_function_variable(
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
