#pragma once

#include <string>
#include <string_view>

#include "rain/lang/lex/location.hpp"

namespace rain::lang::err {

std::tuple<std::string_view /*source_line*/, std::string /*underline*/> underline_source(
    lex::Location location, size_t max_length) noexcept;

}  // namespace rain::lang::err