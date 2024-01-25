#pragma once

#include <string_view>

namespace cirrus::lang {

struct Location {
    int              line   = 0;
    int              column = 0;
    std::string_view source;
};

}  // namespace cirrus::lang
