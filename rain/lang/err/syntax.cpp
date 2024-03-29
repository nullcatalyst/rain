#include "rain/lang/err/syntax.hpp"

#include "absl/strings/str_cat.h"
#include "rain/util/colors.hpp"

namespace rain::lang::err {

std::string SyntaxError::message() const noexcept {
    const auto source_line = _location.whole_line().text();

    std::string under_line;
    under_line.reserve(source_line.size());

    for (int i = 0; i < _location.column - 1; ++i) {
        under_line += ' ';
    }
    for (int i = 0, end = std::min(source_line.size(), _location.text().size()); i < end; ++i) {
        under_line += '~';
    }

    return absl::StrCat(ANSI_BOLD, _location.file_name, ":", _location.line, ":", _location.column,
                        ANSI_RESET, ": ", ANSI_RED, "error: ", ANSI_RESET, ANSI_BOLD, _msg,
                        ANSI_RESET, "\n", source_line, "\n", ANSI_GREEN, under_line, ANSI_RESET,
                        "\n");
}

}  // namespace rain::lang::err
