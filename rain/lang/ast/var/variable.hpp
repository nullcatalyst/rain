#pragma once

#include <memory>
#include <string_view>

#include "rain/lang/ast/type/type.hpp"

namespace rain::lang::ast {

class Variable {
  public:
    virtual ~Variable() = default;

    [[nodiscard]] virtual std::string_view name() const noexcept = 0;
    [[nodiscard]] virtual Type*            type() const noexcept = 0;
};

}  // namespace rain::lang::ast
