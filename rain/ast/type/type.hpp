#pragma once

#include <memory>

#include "rain/lang/location.hpp"

namespace rain::ast {

/**
 * A list of all type kinds.
 */
enum class TypeKind {
    Unknown,

    // <keep-sorted>
    FunctionType,
    InterfaceType,
    OpaqueType,
    StructType,
    TupleType,
    UnresolvedType,
    // </keep-sorted>

    Count,
};

class Type : public std::enable_shared_from_this<Type> {
  protected:
    lang::Location _location;

  public:
    virtual ~Type() = default;

    [[nodiscard]] virtual constexpr TypeKind      kind() const noexcept = 0;
    [[nodiscard]] constexpr const lang::Location& location() const noexcept { return _location; }
};

using TypePtr = std::shared_ptr<Type>;

}  // namespace rain::ast
