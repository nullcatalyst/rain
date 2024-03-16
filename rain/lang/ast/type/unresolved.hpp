#pragma once

#include <string_view>

#include "rain/lang/ast/type/type.hpp"

namespace rain::lang::ast {

/**
 * Represents a named type that has not yet been resolved.
 */
class UnresolvedType : public Type {
    std::string_view _name;

  public:
    UnresolvedType(std::string_view name) : _name(name) {}
    ~UnresolvedType() override = default;
};

}  // namespace rain::lang::ast
