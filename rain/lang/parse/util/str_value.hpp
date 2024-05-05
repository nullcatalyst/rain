#pragma once

#include <string>

#include "rain/lang/lex/token.hpp"
#include "rain/util/result.hpp"

namespace rain::lang::parse {

inline util::Result<std::string> str_value(const lex::Token token) {
    // TODO: Handle escape sequences.
    return std::string(token.location.text().substr(1, token.location.text().size() - 2));
}

}  // namespace rain::lang::parse
