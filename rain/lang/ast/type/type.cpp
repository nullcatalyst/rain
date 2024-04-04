#include "rain/lang/ast/type/type.hpp"

namespace rain::lang::ast {

OptionalType& Type::get_optional_type() {
    if (_optional_type != nullptr) {
        return *_optional_type;
    }
    _optional_type = std::make_unique<OptionalType>(this);
    return *_optional_type;
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

std::string ArrayType::display_name() const noexcept {
    return absl::StrCat("[", _length, "]", _type->display_name());
}

util::Result<absl::Nonnull<Type*>> ArrayType::resolve(Options& options, Scope& scope) {
    auto result = _type->resolve(options, scope);
    FORWARD_ERROR(result);

    auto type = std::move(result).value();
    if (type == _type.get()) {
        return this;
    }

    // If the type was resolved to a different type, then we need to get the correct instance of the
    // array type from the newly resolved type.
    return &type->get_array_type(_length);
}

std::string OptionalType::display_name() const noexcept {
    return absl::StrCat("?", _type->display_name());
}

util::Result<absl::Nonnull<Type*>> OptionalType::resolve(Options& options, Scope& scope) {
    auto result = _type->resolve(options, scope);
    FORWARD_ERROR(result);

    auto type = std::move(result).value();
    if (type == _type.get()) {
        return this;
    }

    // If the type was resolved to a different type, then we need to get the correct instance of the
    // array type from the newly resolved type.
    return &type->get_optional_type();
}

}  // namespace rain::lang::ast
