#include "rain/lang/ast/type/type.hpp"

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

// absl::Nonnull<Type*> _unwrap_type(absl::Nonnull<Type*> type) {
//     return const_cast<absl::Nonnull<Type*>>(
//         _unwrap_type(const_cast<absl::Nonnull<const Type*>>(type)));
// }

OptionalType& Type::get_optional_type() {
    if (_optional_type != nullptr) {
        return *_optional_type;
    }
    _optional_type = std::make_unique<OptionalType>(this);
    return *_optional_type;
}

ReferenceType& Type::get_reference_type() {
    if (_reference_type != nullptr) {
        return *_reference_type;
    }
    _reference_type = std::make_unique<ReferenceType>(this);
    return *_reference_type;
}

ArrayType& Type::get_array_type(size_t length) {
    auto it = _array_types.find(length);
    if (it != _array_types.end()) {
        return *it->second;
    }

    auto  array_type = std::make_unique<ArrayType>(this, length);
    auto& ref        = *array_type;
    _array_types.emplace(length, std::move(array_type));
    return ref;
}

////////////////////////////////////////////////////////////////
// ArrayType

ArrayType::ArrayType(absl::Nonnull<Type*> type, size_t length) : _type(type), _length(length) {}

ArrayType::ArrayType(absl::Nonnull<Type*> type, size_t length, lex::Location location)
    : _type(type), _length(length), _location(location) {}

std::string ArrayType::display_name() const noexcept {
    return absl::StrCat("[", _length, "]", _type->display_name());
}

util::Result<absl::Nonnull<Type*>> ArrayType::resolve(Options& options, Scope& scope) {
    {
        auto result = _type->resolve(options, scope);
        FORWARD_ERROR(result);

        _type = std::move(result).value();
    }

    return &_type->get_array_type(_length);
}

////////////////////////////////////////////////////////////////
// OptionalType

OptionalType::OptionalType(absl::Nonnull<Type*> type) : _type(type) {}

OptionalType::OptionalType(absl::Nonnull<Type*> type, lex::Location location)
    : _type(std::move(type)), _location(location) {}

std::string OptionalType::display_name() const noexcept {
    return absl::StrCat("?", _type->display_name());
}

util::Result<absl::Nonnull<Type*>> OptionalType::resolve(Options& options, Scope& scope) {
    {
        auto result = _type->resolve(options, scope);
        FORWARD_ERROR(result);

        _type = std::move(result).value();
    }

    return &_type->get_optional_type();
}

////////////////////////////////////////////////////////////////
// ReferenceType

ReferenceType::ReferenceType(absl::Nonnull<Type*> type) : _type(type) {}

ReferenceType::ReferenceType(absl::Nonnull<Type*> type, lex::Location location)
    : _type(std::move(type)), _location(location) {}

std::string ReferenceType::display_name() const noexcept {
    return absl::StrCat("&", _type->display_name());
}

util::Result<absl::Nonnull<Type*>> ReferenceType::resolve(Options& options, Scope& scope) {
    {
        auto result = _type->resolve(options, scope);
        FORWARD_ERROR(result);

        _type = std::move(result).value();
    }

    return &_type->get_reference_type();
}

}  // namespace rain::lang::ast
