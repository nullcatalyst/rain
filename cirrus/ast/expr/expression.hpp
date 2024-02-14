#pragma once

#include "cirrus/ast/node.hpp"
#include "cirrus/ast/type/type.hpp"

namespace cirrus::ast {

class Expression : public Node {
  protected:
    TypePtr _type;

  public:
    ~Expression() override = default;

    [[nodiscard]] constexpr const TypePtr& type() const noexcept { return _type; }
    void set_type(TypePtr type) noexcept { _type = std::move(type); }

    [[nodiscard]] virtual bool compile_time_capable() const noexcept = 0;
};

using ExpressionPtr = std::shared_ptr<Expression>;

}  // namespace cirrus::ast
