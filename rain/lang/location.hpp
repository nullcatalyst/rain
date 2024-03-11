#pragma once

#include <string_view>

#include "rain/util/colors.hpp"
#include "rain/util/log.hpp"

namespace rain::lang {

class Location {
  public:
    std::string_view _source;
    const char*      _begin  = 0;
    const char*      _end    = 0;
    int              _line   = 0;
    int              _column = 0;

  public:
    Location() = default;
    Location(const std::string_view source, const char* begin, const char* end, int line,
             int column)
        : _source(source), _begin(begin), _end(end), _line(line), _column(column) {}

    [[nodiscard]] constexpr int line() const noexcept { return _line; }
    [[nodiscard]] constexpr int column() const noexcept { return _column; }

    [[nodiscard]] std::string_view substr() const noexcept {
        return std::string_view{_begin, _end};
    }

    [[nodiscard]] Location merge(const Location& other) const noexcept {
#if !defined(NDEBUG) && !RAIN_USE_TWINE
        if (_source != other._source) {
            util::console_error(ANSI_RED, "cannot merge locations from different sources",
                                ANSI_RESET);
            std::abort();
        }
#endif  // !defined(NDEBUG)

        const char* const begin = std::min(_begin, other._begin);
        const char* const end   = std::max(_end, other._end);
        return Location(_source, begin, end, std::min(_line, other._line),
                        std::min(_column, other._column));
    }

    [[nodiscard]] Location whole_line() const noexcept {
        const char* line_start = _begin;
        while (line_start > _source.data() && *line_start != '\n') {
            --line_start;
        }

        const char* line_end = _begin;
        while (line_end < _source.data() + _source.size() && *line_end != '\n') {
            ++line_end;
        }

        return Location(_source, line_start, line_end, _line, 1);
    }

    // Return the empty string after the location.
    // This is useful for error messages where we want to say that a specific token needs to follow
    // the previous token, or when encountering the end of the file.
    [[nodiscard]] Location empty_string_after() const {
        return Location(_source, _end, _end, _line, _column + 1);
    }
};

}  // namespace rain::lang
