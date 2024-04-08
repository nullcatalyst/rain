#include "rain/lang/err/multiple_definition.hpp"

#include "absl/strings/str_cat.h"
#include "rain/lang/err/underline.hpp"
#include "rain/util/colors.hpp"

namespace rain::lang::err {

std::string MultipleDefinitionError::message() const noexcept {
    const auto [second_source_line, second_underline] = underline_source(_second_location, -1);
    const auto [first_source_line, first_underline]   = underline_source(_first_location, -1);

    return absl::StrCat(ANSI_BOLD, _second_location.file_name, ":", _second_location.line, ":",
                        _second_location.column, ANSI_RESET, ": ", ANSI_RED, "error: ", ANSI_RESET,
                        ANSI_BOLD, _msg, ANSI_RESET, "\n", second_source_line, "\n", ANSI_GREEN,
                        second_underline, ANSI_RESET, "\n",

                        ANSI_BOLD, _first_location.file_name, ":", _first_location.line, ":",
                        _first_location.column, ANSI_RESET, ": ", ANSI_RED, "error: ", ANSI_RESET,
                        ANSI_BOLD, "previous declaration here", ANSI_RESET, "\n", first_source_line,
                        "\n", ANSI_GREEN, first_underline, ANSI_RESET, "\n");
}

}  // namespace rain::lang::err
