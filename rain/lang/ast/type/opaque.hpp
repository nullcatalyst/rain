#pragma once

#include "rain/lang/ast/type/type.hpp"

namespace rain::lang::ast {

/**
 * Represents an opaque type, where the implementation is hidden from the user of the language.
 *
 * Some examples of opaque types include the primitive types (`i32`, `f32`, `bool`, etc.) where
 * they cannot be defined within the language itself.
 */
class OpaqueType : public Type {
  public:
    ~OpaqueType() override = default;
};

}  // namespace rain::lang::ast
