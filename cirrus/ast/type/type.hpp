#pragma once

#include <memory>

#include "cirrus/ast/node.hpp"

namespace cirrus::ast {

class Type : public Node {
  public:
    ~Type() override = default;
};

using TypePtr = std::shared_ptr<Type>;

}  // namespace cirrus::ast
