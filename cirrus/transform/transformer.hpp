#pragma once

#include "cirrus/ast/node.hpp"

namespace cirrus::transform {

class Transformer {
  public:
    Transformer()  = default;
    ~Transformer() = default;

    ast::Node transform(ast::Node node);
};

}  // namespace cirrus::transform
