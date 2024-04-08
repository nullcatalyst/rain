#include "rain/lang/err/syntax.hpp"

#include "absl/strings/str_cat.h"
#include "rain/lang/err/underline.hpp"
#include "rain/util/colors.hpp"

namespace rain::lang::err {

std::string SyntaxError::message() const noexcept {
    const auto [source_line, underline] = underline_source(_location, -1);

    return absl::StrCat(ANSI_BOLD, _location.file_name, ":", _location.line, ":", _location.column,
                        ANSI_RESET, ": ", ANSI_RED, "error: ", ANSI_RESET, ANSI_BOLD, _msg,
                        ANSI_RESET, "\n", source_line, "\n", ANSI_GREEN, underline, ANSI_RESET,
                        "\n");
}

}  // namespace rain::lang::err
