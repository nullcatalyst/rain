#pragma once

#include <cstddef>
#include <limits>

#include "rain/lang/lex/token.hpp"
#include "rain/util/result.hpp"

namespace rain::lang::parse {

template <typename T>
util::Result<T> int_value(const lex::Token token) {
    static constexpr const size_t MAX_INT_PRE_MULTIPLY = std::numeric_limits<size_t>::max() / 10;
    static constexpr const size_t MAX_INT_PRE_ADD      = std::numeric_limits<size_t>::max() - 9;

    T value = 0;
    for (const auto c : token.location.text()) {
        if (value > MAX_INT_PRE_MULTIPLY) {
            // Multiplying by 10 will overflow.
            return ERR_PTR(err::SyntaxError, token.location, "integer literal is too large");
        }
        value *= 10;

        if (value > MAX_INT_PRE_ADD) {
            // Adding the next digit will overflow.
            return ERR_PTR(err::SyntaxError, token.location, "integer literal is too large");
        }
        value += c - '0';
    }

    return value;
}

}  // namespace rain::lang::parse
