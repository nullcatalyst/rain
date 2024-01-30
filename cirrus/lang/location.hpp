#pragma once

#include <string_view>

namespace cirrus::lang {

struct Location {
    int              line   = 0;
    int              column = 0;
    std::string_view source;

    constexpr Location merge(const Location& other) const noexcept {
        const char* const begin = std::min(source.begin(), other.source.begin());
        const char* const end   = std::max(source.end(), other.source.end());
        return Location{
            .line   = std::min(line, other.line),
            .column = std::min(column, other.column),
            .source = std::string_view(begin, end),
        };
    }
};

}  // namespace cirrus::lang
