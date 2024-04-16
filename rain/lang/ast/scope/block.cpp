#include "rain/lang/ast/scope/block.hpp"

namespace rain::lang::ast {

BlockScope::BlockScope(Scope& parent) : _parent(parent), _module(*parent.module()) {}

BlockScope::BlockScope(BlockScope&& other)
    : Scope(std::move(other)), _parent(other._parent), _module(other._module) {}

}  // namespace rain::lang::ast
