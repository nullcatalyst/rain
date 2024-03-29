#include "rain/lang/err/syntax.hpp"

#include "absl/strings/str_cat.h"
#include "rain/util/colors.hpp"

namespace rain::lang::err {

std::string SyntaxError::message() const noexcept {
    const auto  source_line = _location.whole_line().text();
    int         capacity    = source_line.size();
    std::string under_line;
    under_line.reserve(capacity);

    // Column indexing starts at 1, whereas capacity is indexed from 0.
    // Offset by 1 to make them match.
    capacity += 1;

    int column = 1;
    for (int end = std::min(capacity, _location.column); column < end; ++column) {
        under_line += ' ';
    }
    for (int end = std::min(capacity, _location.column + static_cast<int>(_location.text().size()));
         column < end; ++column) {
        under_line += '~';
    }

    return absl::StrCat(ANSI_BOLD, _location.file_name, ":", _location.line, ":", _location.column,
                        ANSI_RESET, ": ", ANSI_RED, "error: ", ANSI_RESET, ANSI_BOLD, _msg,
                        ANSI_RESET, "\n", source_line, "\n", ANSI_GREEN, under_line, ANSI_RESET,
                        "\n");
}

}  // namespace rain::lang::err
