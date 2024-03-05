#pragma once

#include "rain/util/colors.hpp"
#include "rain/util/log.hpp"
#include "rain/util/string.hpp"

namespace rain::lang {

class Location {
  public:
    util::String         _source;
    util::StringIterator _begin  = 0;
    util::StringIterator _end    = 0;
    int                  _line   = 0;
    int                  _column = 0;

  public:
    Location() = default;
    Location(util::String source, const util::StringIterator begin, const util::StringIterator end,
             int line, int column)
        : _source(source), _begin(begin), _end(end), _line(line), _column(column) {}

    [[nodiscard]] constexpr int line() const noexcept { return _line; }
    [[nodiscard]] constexpr int column() const noexcept { return _column; }

    [[nodiscard]] util::String substr() const noexcept {
        return _source.substr(_begin, _end - _begin);
    }

    [[nodiscard]] Location merge(const Location& other) const noexcept {
#if !defined(NDEBUG) && !RAIN_USE_TWINE
        if (_source != other._source) {
            util::console_error(ANSI_RED, "cannot merge locations from different sources",
                                ANSI_RESET);
            std::abort();
        }
#endif  // !defined(NDEBUG)

        const util::StringIterator begin = std::min(_begin, other._begin);
        const util::StringIterator end   = std::max(_end, other._end);
        return Location(_source, begin, end, std::min(_line, other._line),
                        std::min(_column, other._column));
    }

    [[nodiscard]] Location whole_line() const noexcept {
        if (_begin < 0) {
            // The source location is past the end of the file
            return *this;
        }

        auto line_start = _source.rfind('\n', _source[_begin] == '\n' ? _begin - 1 : _begin);
        if (line_start < 0) {
            // The source location is at the start of the file
            line_start = 0;
        } else {
            // Skip the newline character
            ++line_start;
        }

        auto line_end = _source.find('\n', _begin);
        if (line_end < 0) {
            // The source location is at the end of the file
            line_end = _source.size();
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
