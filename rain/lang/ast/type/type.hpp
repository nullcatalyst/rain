#pragma once

#include <memory>
#include <string>

#include "absl/base/nullability.h"
#include "absl/container/flat_hash_map.h"
#include "rain/lang/lex/location.hpp"
#include "rain/lang/options.hpp"
#include "rain/lang/serial/kind.hpp"
#include "rain/util/result.hpp"

namespace rain::lang::ast {

class Scope;
class Expression;

class ArrayType;
class OptionalType;
class ReferenceType;

class Type {
  protected:
    std::unique_ptr<OptionalType>                           _optional_type;
    std::unique_ptr<ReferenceType>                          _reference_type;
    absl::flat_hash_map<size_t, std::unique_ptr<ArrayType>> _array_types;

    std::vector<absl::Nonnull<Type*>> _interface_implementations;
    absl::Nullable<Type*>             _resolves_to = nullptr;

  public:
    static Type& unwrap(Type& type) noexcept;

    virtual ~Type() = default;

    [[nodiscard]] virtual serial::TypeKind kind() const noexcept = 0;
    [[nodiscard]] virtual std::string      display_name() const noexcept { return "<error_type>"; }
    [[nodiscard]] virtual lex::Location    location() const noexcept { return lex::Location(); }

    [[nodiscard]] virtual ArrayType&     get_array_type(Scope& scope, size_t length);
    [[nodiscard]] virtual OptionalType&  get_optional_type(Scope& scope);
    [[nodiscard]] virtual ReferenceType& get_reference_type(Scope& scope);

    [[nodiscard]] constexpr const auto& array_types() const noexcept { return _array_types; }

    /**
     * Resolve the type to a concrete (and common) type. Fully resolved types can be compared using
     * pointer equality.
     *
     * The instance used to call this method is no longer valid after calling this method. If the
     * instance was being stored, then it should be replaced with the returned value.
     */
    [[nodiscard]] util::Result<absl::Nonnull<Type*>> resolve(Options& options, Scope& scope);

  protected:
    [[nodiscard]] virtual util::Result<absl::Nonnull<Type*>> _resolve(Options& options,
                                                                      Scope&   scope) = 0;
};

class ArrayType : public Type {
    absl::Nonnull<Type*> _type;
    size_t               _length;

    lex::Location _location;

  public:
    ArrayType(absl::Nonnull<Type*> type, size_t length);
    ArrayType(absl::Nonnull<Type*> type, size_t length, lex::Location location);
    ~ArrayType() override = default;

    [[nodiscard]] constexpr serial::TypeKind kind() const noexcept override {
        return serial::TypeKind::Array;
    }
    [[nodiscard]] constexpr lex::Location location() const noexcept override { return _location; }
    [[nodiscard]] std::string             display_name() const noexcept override;

    [[nodiscard]] constexpr size_t      length() const noexcept { return _length; }
    [[nodiscard]] constexpr const Type& type() const noexcept { return *_type; }
    [[nodiscard]] constexpr Type&       type() noexcept { return *_type; }

  protected:
    [[nodiscard]] util::Result<absl::Nonnull<Type*>> _resolve(Options& options,
                                                              Scope&   scope) override;
};

class OptionalType : public Type {
    absl::Nonnull<Type*> _type;

    lex::Location _location;

  public:
    OptionalType(absl::Nonnull<Type*> type);
    OptionalType(absl::Nonnull<Type*> type, lex::Location location);
    ~OptionalType() override = default;

    [[nodiscard]] constexpr serial::TypeKind kind() const noexcept override {
        return serial::TypeKind::Optional;
    }
    [[nodiscard]] constexpr lex::Location location() const noexcept override { return _location; }
    [[nodiscard]] std::string             display_name() const noexcept override;

    [[nodiscard]] constexpr const Type& type() const noexcept { return *_type; }
    [[nodiscard]] constexpr Type&       type() noexcept { return *_type; }

  protected:
    [[nodiscard]] util::Result<absl::Nonnull<Type*>> _resolve(Options& options,
                                                              Scope&   scope) override;
};

class ReferenceType : public Type {
    absl::Nonnull<Type*> _type;

    lex::Location _location;

  public:
    ReferenceType(absl::Nonnull<Type*> type);
    ReferenceType(absl::Nonnull<Type*> type, lex::Location location);
    ~ReferenceType() override = default;

    [[nodiscard]] constexpr serial::TypeKind kind() const noexcept override {
        return serial::TypeKind::Reference;
    }
    [[nodiscard]] constexpr lex::Location location() const noexcept override { return _location; }
    [[nodiscard]] std::string             display_name() const noexcept override;

    [[nodiscard]] constexpr const Type& type() const noexcept { return *_type; }
    [[nodiscard]] constexpr Type&       type() noexcept { return *_type; }

  protected:
    [[nodiscard]] util::Result<absl::Nonnull<Type*>> _resolve(Options& options,
                                                              Scope&   scope) override;
};

}  // namespace rain::lang::ast
