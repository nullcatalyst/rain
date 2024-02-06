#pragma once

#include <vector>

#include "cirrus/ast/node.hpp"

namespace cirrus::lang {

class Validator;
class Compiler;

class Module {
    std::vector<ast::Node> _nodes;

  public:
    Module() = default;
    Module(std::vector<ast::Node> nodes) : _nodes(std::move(nodes)) {}

    [[nodiscard]] constexpr const std::vector<ast::Node>& nodes() const noexcept { return _nodes; }
    [[nodiscard]] constexpr std::vector<ast::Node>&       nodes() noexcept { return _nodes; }
};

// Use compile-time type information to ensure that only valid modules are compilable

class ParsedModule : public Module {
    friend class ::cirrus::lang::Validator;

  public:
    using Module::Module;
};

class ValidatedModule : public Module {
    friend class ::cirrus::lang::Compiler;

  public:
    using Module::Module;
};

}  // namespace cirrus::lang
