#include "rain/lang/err/underline.hpp"

#include <cmath>

namespace rain::lang::err {

std::tuple<std::string_view /*source_line*/, std::string /*underline*/> underline_source(
    lex::Location location, size_t max_length) noexcept {
    const auto  source_line = location.whole_line().text();
    int         capacity    = source_line.size();
    std::string underline;
    underline.reserve(capacity);

    // Column indexing starts at 1, whereas capacity is indexed from 0.
    // Offset by 1 to make them match.
    capacity += 1;

    int column = 1;
    for (int end = std::min(capacity, location.column); column < end; ++column) {
        underline += ' ';
    }
    for (int end = std::min(capacity, location.column + static_cast<int>(location.text().size()));
         column < end; ++column) {
        underline += '~';
    }

    return std::make_tuple(source_line, std::move(underline));
}

}  // namespace rain::lang::err
