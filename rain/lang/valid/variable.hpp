#pragma once

#include <string_view>

namespace rain::lang::valid {

class Variable {
    std::string_view _name;
    ast::TypePtr     _type;

  public:
    Variable()  = default;
    ~Variable() = default;

    [[nodiscard]] constexpr std::string_view name() const noexcept { return _name; }
    [[nodiscard]] constexpr ast::TypePtr     type() const noexcept { return _type; }
};

}  // namespace rain::lang::valid
