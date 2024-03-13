#pragma once

#include <cstdint>

namespace rain::lang::ast {

class Expression {
    uint32_t _start_token_index;
    uint32_t _end_token_index;

  public:
    virtual ~Expression() = default;

    [[nodiscard]] constexpr uint32_t start_token_index() const noexcept {
        return _start_token_index;
    }
    [[nodiscard]] constexpr uint32_t end_token_index() const noexcept { return _end_token_index; }
};

}  // namespace rain::lang::ast
