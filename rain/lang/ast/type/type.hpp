#pragma once

#include <memory>

namespace rain::lang::ast {

class Type {
  public:
    virtual ~Type() = default;
};

using TypePtr = std::shared_ptr<Type>;

}  // namespace rain::lang::ast
