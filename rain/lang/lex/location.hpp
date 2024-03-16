#pragma once

#include <string_view>

#include "rain/util/colors.hpp"
#include "rain/util/log.hpp"

namespace rain::lang::lex {

struct Location {
    const char*      begin  = 0;
    const char*      end    = 0;
    int              line   = 0;
    int              column = 0;
    std::string_view source;

  public:
    Location() = default;
    Location(const std::string_view source, const char* begin, const char* end, int line,
             int column)
        : begin(begin), end(end), line(line), column(column), source(source) {}

#if defined(_MSC_VER)
    // The MSVC stdlib hdrs don't appear to allow implicit conversion from string_view::iterator to
    // const char* or vice versa. So explicitly add an overload for MSVC.
    Location(const std::string_view source, const std::string_view::iterator begin,
             const std::string_view::iterator end, int line, int column)
        : begin(&*begin), end(&*end), line(line), column(column), source(source) {}
#endif  // defined(_MSC_VER)

    [[nodiscard]] std::string_view text() const noexcept { return std::string_view{begin, end}; }

    [[nodiscard]] Location merge(const Location& other) const noexcept {
#if !defined(NDEBUG)
        if (source.data() != other.source.data() && source.size() != other.source.size()) {
            util::console_error(ANSI_RED, "cannot merge locations from different sources",
                                ANSI_RESET);
            std::abort();
        }
#endif  // !defined(NDEBUG)

        return Location(source, std::min(begin, other.begin), std::max(end, other.end),
                        std::min(line, other.line), std::min(column, other.column));
    }

    [[nodiscard]] Location whole_line() const noexcept {
        const char* line_start = begin;
        while (line_start > source.data() && *line_start != '\n') {
            --line_start;
        }

        const char* line_end = begin;
        while (line_end < source.data() + source.size() && *line_end != '\n') {
            ++line_end;
        }

        return Location(source, line_start, line_end, line, 1);
    }

    // Return the empty string after the location.
    // This is useful for error messages where we want to say that a specific token needs to follow
    // the previous token, or when encountering the end of the file.
    [[nodiscard]] Location empty_string_after() const {
        return Location(source, end, end, line, column + 1);
    }
};

}  // namespace rain::lang::lex
