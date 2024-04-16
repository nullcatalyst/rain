#include "rain/lang/err/return_type_mismatch.hpp"

#include "absl/strings/str_cat.h"
#include "rain/lang/err/underline.hpp"
#include "rain/util/colors.hpp"

namespace rain::lang::err {

std::string ReturnTypeMismatchError::message() const noexcept {
    const auto [return_source_line, return_underline] = underline_source(_return_location, -1);
    const auto [declaration_source_line, declaration_underline] =
        underline_source(_declaration_location, -1);

    return absl::StrCat(
        ANSI_BOLD, _return_location.file_name, ":", _return_location.line, ":",
        _return_location.column, ANSI_RESET, ": ", ANSI_RED, "error: ", ANSI_RESET, ANSI_BOLD,
        "return type mismatch, function was declared with return type '", _delcaration_type_name,
        "' but was found to return type '", _return_type_name, "' instead", ANSI_RESET, "\n",
        return_source_line, "\n", ANSI_GREEN, return_underline, ANSI_RESET, "\n",

        ANSI_BOLD, _declaration_location.file_name, ":", _declaration_location.line, ":",
        _declaration_location.column, ANSI_RESET, ": ", ANSI_RED, "error: ", ANSI_RESET, ANSI_BOLD,
        "expected return type declared here", ANSI_RESET, "\n", declaration_source_line, "\n",
        ANSI_GREEN, declaration_underline, ANSI_RESET, "\n");
}

}  // namespace rain::lang::err
