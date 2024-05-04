#include "rain/lang/err/if_type_mismatch.hpp"

#include "absl/strings/str_cat.h"
#include "rain/lang/err/underline.hpp"
#include "rain/util/colors.hpp"

namespace rain::lang::err {

std::string IfTypeMismatchError::message() const noexcept {
    const auto [then_value_source_line, then_value_underline] =
        underline_source(_then_value_location, -1);
    const auto [else_value_source_line, else_value_underline] =
        underline_source(_else_value_location, -1);

    return absl::StrCat(
        ANSI_BOLD, _then_value_location.file_name, ":", _then_value_location.line, ":",
        _then_value_location.column, ANSI_RESET, ": ", ANSI_RED, "error: ", ANSI_RESET, ANSI_BOLD,
        "if branches result in differing types; then branch has type '", _then_value_type_name,
        "', and the else branch has type '", _else_value_type_name, "' instead", ANSI_RESET, "\n",
        then_value_source_line, "\n", ANSI_GREEN, then_value_underline, ANSI_RESET, "\n",

        ANSI_BOLD, _else_value_location.file_name, ":", _else_value_location.line, ":",
        _else_value_location.column, ANSI_RESET, ": ", ANSI_RED, "error: ", ANSI_RESET, ANSI_BOLD,
        "else branch has value here", ANSI_RESET, "\n", else_value_source_line, "\n", ANSI_GREEN,
        else_value_underline, ANSI_RESET, "\n");
}

}  // namespace rain::lang::err
