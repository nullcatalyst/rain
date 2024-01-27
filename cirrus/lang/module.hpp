#pragma once

#include <string_view>
#include <vector>

#include "cirrus/ast/node.hpp"

namespace cirrus::lang {

class Module {
    std::vector<ast::Node> _nodes;

  public:
    Module() = default;
    Module(std::vector<ast::Node> nodes) : _nodes(std::move(nodes)) {}

    [[nodiscard]] constexpr const std::vector<ast::Node>& nodes() const noexcept { return _nodes; }
    [[nodiscard]] constexpr std::vector<ast::Node>&       nodes() noexcept { return _nodes; }
};

}  // namespace cirrus::lang
