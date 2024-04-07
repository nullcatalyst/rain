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

void Type::add_ref(Expression& expression) noexcept {
    // Do nothing.
}

void Type::remove_ref(Expression& expression) noexcept {
    // Do nothing.
}

void Type::add_ref(Type& type) noexcept {
    // Do nothing.
}

void Type::remove_ref(Type& type) noexcept {
    // Do nothing.
}

void Type::replace_type(absl::Nonnull<Type*> old_type, absl::Nonnull<Type*> new_type) {
    util::panic("replacing type '", display_name(),
                "' is not implemented; this is an internal error");
}

////////////////////////////////////////////////////////////////
// ArrayType

ArrayType::ArrayType(util::MaybeOwnedPtr<Type> type, size_t length)
    : _type(std::move(type)), _length(length) {
    _type->add_ref(*this);
}

ArrayType::ArrayType(util::MaybeOwnedPtr<Type> type, size_t length, lex::Location location)
    : _type(std::move(type)), _length(length), _location(location) {
    _type->add_ref(*this);
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

void ArrayType::replace_type(absl::Nonnull<Type*> old_type, absl::Nonnull<Type*> new_type) {
    IF_DEBUG {
        if (_type.get() != old_type) {
            util::panic(
                "attempted to replace the type of an array type, but the type being replaced "
                "is not the type of the array type");
        }
    }

    _type = new_type;

    IF_DEBUG {
        // Only remove the ref in debug/test builds, so that a (potentially) expensive search isn't
        // needed for every replaced instance of every type.
        old_type->remove_ref(*this);
    }
}

absl::Nonnull<Type*> ArrayType::should_be_replaced_with(Scope& scope) noexcept {
    return &_type->get_array_type(_length);
}

////////////////////////////////////////////////////////////////
// OptionalType

OptionalType::OptionalType(util::MaybeOwnedPtr<Type> type) : _type(std::move(type)) {
    _type->add_ref(*this);
}

OptionalType::OptionalType(util::MaybeOwnedPtr<Type> type, lex::Location location)
    : _type(std::move(type)), _location(location) {
    _type->add_ref(*this);
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

void OptionalType::replace_type(absl::Nonnull<Type*> old_type, absl::Nonnull<Type*> new_type) {
    IF_DEBUG {
        if (_type.get() != old_type) {
            util::panic(
                "attempted to replace the type of an optional type, but the type being replaced "
                "is not the type of the optional type");
        }
    }

    _type = new_type;

    IF_DEBUG {
        // Only remove the ref in debug/test builds, so that a (potentially) expensive search isn't
        // needed for every replaced instance of every type.
        old_type->remove_ref(*this);
    }
}

absl::Nonnull<Type*> OptionalType::should_be_replaced_with(Scope& scope) noexcept {
    return &_type->get_optional_type();
}

}  // namespace rain::lang::ast
