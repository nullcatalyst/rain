#pragma once

#include <memory>
#include <string>

#include "absl/base/nullability.h"
#include "absl/container/flat_hash_map.h"
#include "rain/lang/lex/location.hpp"
#include "rain/lang/options.hpp"
#include "rain/lang/serial/kind.hpp"
#include "rain/util/maybe_owned_ptr.hpp"
#include "rain/util/result.hpp"

namespace rain::lang::ast {

class Scope;

class ArrayType;
class OptionalType;

class Type {
  protected:
    std::unique_ptr<OptionalType>                           _optional_type;
    absl::flat_hash_map<size_t, std::unique_ptr<ArrayType>> _array_types;

  public:
    static std::string type_name(absl::Nullable<Type*> type) noexcept {
        if (type == nullptr) {
            return "<null_type>";
        }
        return type->name();
    }

    virtual ~Type() = default;

    [[nodiscard]] virtual serial::TypeKind kind() const noexcept = 0;
    [[nodiscard]] virtual std::string      name() const noexcept { return "<error_type>"; }
    [[nodiscard]] virtual lex::Location    location() const noexcept { return lex::Location(); }

    [[nodiscard]] OptionalType& get_optional_type();
    [[nodiscard]] ArrayType&    get_array_type(size_t length);

    [[nodiscard]] constexpr const absl::flat_hash_map<size_t, std::unique_ptr<ArrayType>>&
    array_types() const noexcept {
        return _array_types;
    }

    /**
     * Resolve the type to a concrete (and common) type. Fully resolved types can be compared using
     * pointer equality.
     *
     * The instance used to call this method is no longer valid after calling this method. If the
     * instance was being stored, then it should be replaced with the returned value.
     */
    [[nodiscard]] virtual util::Result<absl::Nonnull<Type*>> resolve(Options& options,
                                                                     Scope&   scope) = 0;
};

class ArrayType : public Type {
    util::MaybeOwnedPtr<Type> _type;
    size_t                    _length;

    lex::Location _location;

  public:
    ArrayType(util::MaybeOwnedPtr<Type> type, size_t length)
        : _type(std::move(type)), _length(length) {}
    ArrayType(util::MaybeOwnedPtr<Type> type, size_t length, lex::Location location)
        : _type(std::move(type)), _length(length), _location(location) {}
    ~ArrayType() override = default;

    [[nodiscard]] constexpr serial::TypeKind kind() const noexcept override {
        return serial::TypeKind::Array;
    }
    [[nodiscard]] constexpr lex::Location location() const noexcept override { return _location; }
    [[nodiscard]] std::string             name() const noexcept override;

    [[nodiscard]] constexpr size_t      length() const noexcept { return _length; }
    [[nodiscard]] constexpr const Type& type() const noexcept { return *_type.get(); }
    [[nodiscard]] constexpr Type&       type() noexcept { return *_type.get(); }

    [[nodiscard]] util::Result<absl::Nonnull<Type*>> resolve(Options& options,
                                                             Scope&   scope) override;
};

class OptionalType : public Type {
    util::MaybeOwnedPtr<Type> _type;

    lex::Location _location;

  public:
    OptionalType(util::MaybeOwnedPtr<Type> type) : _type(std::move(type)) {}
    OptionalType(util::MaybeOwnedPtr<Type> type, lex::Location location)
        : _type(std::move(type)), _location(location) {}
    ~OptionalType() override = default;

    [[nodiscard]] constexpr serial::TypeKind kind() const noexcept override {
        return serial::TypeKind::Optional;
    }
    [[nodiscard]] constexpr lex::Location location() const noexcept override { return _location; }
    [[nodiscard]] std::string             name() const noexcept override;

    [[nodiscard]] constexpr const Type& type() const noexcept { return *_type.get(); }
    [[nodiscard]] constexpr Type&       type() noexcept { return *_type.get(); }

    [[nodiscard]] util::Result<absl::Nonnull<Type*>> resolve(Options& options,
                                                             Scope&   scope) override;
};

}  // namespace rain::lang::ast
