#pragma once

#include "rain/lang/token.hpp"

namespace rain::lang {

class Lexer {
  public:
    virtual ~Lexer() = default;

    [[nodiscard]] virtual std::string_view source() const noexcept    = 0;
    [[nodiscard]] virtual std::string_view file_name() const noexcept = 0;

    virtual Token next() = 0;
    virtual Token peek() = 0;
};

}  // namespace rain::lang
