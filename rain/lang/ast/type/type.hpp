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
class Expression;

class ArrayType;
class OptionalType;

class Type {
  protected:
    std::unique_ptr<OptionalType>                           _optional_type;
    absl::flat_hash_map<size_t, std::unique_ptr<ArrayType>> _array_types;

    std::vector<absl::Nonnull<Type*>> _interface_implementations;

  public:
    static std::string type_name(absl::Nullable<Type*> type) noexcept {
        if (type == nullptr) {
            return "<null_type>";
        }
        return type->display_name();
    }

    virtual ~Type() = default;

    [[nodiscard]] virtual serial::TypeKind kind() const noexcept = 0;
    [[nodiscard]] virtual std::string      display_name() const noexcept { return "<error_type>"; }
    [[nodiscard]] virtual lex::Location    location() const noexcept { return lex::Location(); }

    [[nodiscard]] OptionalType& get_optional_type();
    [[nodiscard]] ArrayType&    get_array_type(size_t length);

    virtual void add_ref(Expression& expression) noexcept;
    virtual void remove_ref(Expression& expression) noexcept;
    virtual void add_ref(Type& type) noexcept;
    virtual void remove_ref(Type& type) noexcept;

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

    /**
     * When a type contains a reference to an unresolved type, when the scope is being validated,
     * this method will be called with the unresolved type, and a pointer to the new type that it
     * should be replaced with.
     */
    virtual void replace_type(absl::Nonnull<Type*> old_type, absl::Nonnull<Type*> new_type);

    /**
     * When either a type or an expression is being fully resolved (after unresolved types have been
     * dealt with and replaced), this method will be called to replace duplicate type instances with
     * the singleton type instance (for example, to ensure that there is only a single optional type
     * of a type).
     */
    [[nodiscard]] virtual absl::Nonnull<Type*> should_be_replaced_with(Scope& scope) noexcept {
        return this;
    }
};

class ArrayType : public Type {
    util::MaybeOwnedPtr<Type> _type;
    size_t                    _length;

    lex::Location _location;

  public:
    ArrayType(util::MaybeOwnedPtr<Type> type, size_t length);
    ArrayType(util::MaybeOwnedPtr<Type> type, size_t length, lex::Location location);
    ~ArrayType() override = default;

    [[nodiscard]] constexpr serial::TypeKind kind() const noexcept override {
        return serial::TypeKind::Array;
    }
    [[nodiscard]] constexpr lex::Location location() const noexcept override { return _location; }
    [[nodiscard]] std::string             display_name() const noexcept override;

    [[nodiscard]] constexpr size_t          length() const noexcept { return _length; }
    [[nodiscard]] /*constexpr*/ const Type& type() const noexcept { return *_type.get(); }
    [[nodiscard]] /*constexpr*/ Type&       type() noexcept { return *_type.get(); }

    [[nodiscard]] util::Result<absl::Nonnull<Type*>> resolve(Options& options,
                                                             Scope&   scope) override;

    void replace_type(absl::Nonnull<Type*> old_type, absl::Nonnull<Type*> new_type) override;
    [[nodiscard]] absl::Nonnull<Type*> should_be_replaced_with(Scope& scope) noexcept override;
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
    [[nodiscard]] std::string             display_name() const noexcept override;

    [[nodiscard]] /*constexpr*/ const Type& type() const noexcept { return *_type.get(); }
    [[nodiscard]] /*constexpr*/ Type&       type() noexcept { return *_type.get(); }

    [[nodiscard]] util::Result<absl::Nonnull<Type*>> resolve(Options& options,
                                                             Scope&   scope) override;

    void replace_type(absl::Nonnull<Type*> old_type, absl::Nonnull<Type*> new_type) override;
    [[nodiscard]] absl::Nonnull<Type*> should_be_replaced_with(Scope& scope) noexcept override;
};

}  // namespace rain::lang::ast
