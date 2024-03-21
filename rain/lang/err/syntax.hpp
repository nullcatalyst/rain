#pragma once

#include <string>

#include "absl/strings/str_cat.h"
#include "rain/lang/lex/lexer.hpp"
#include "rain/lang/lex/location.hpp"
#include "rain/util/colors.hpp"
#include "rain/util/result.hpp"

namespace rain::lang::err {

class SyntaxError : public util::Error {
    const lex::Lexer& _lexer;
    lex::Location     _location;
    std::string       _msg;

  public:
    SyntaxError(const lex::Lexer& lexer, lex::Location location, std::string msg)
        : _lexer(lexer), _location(location), _msg(std::move(msg)) {}
    ~SyntaxError() override = default;

    [[nodiscard]] std::string message() const noexcept override {
        const auto source_line = _location.whole_line().text();

        std::string under_line;
        under_line.reserve(source_line.size() + ANSI_GREEN.size() + ANSI_RESET.size());

        under_line += ANSI_GREEN;
        for (int i = 0; i < _location.column - 1; ++i) {
            under_line += ' ';
        }
        under_line += '^';
        for (int i = 0; i < static_cast<int>(_location.text().size()) - 1; ++i) {
            under_line += '~';
        }
        under_line += ANSI_RESET;

#define RAIN_PRINT_ERROR_MULTILINE 0
#if RAIN_PRINT_ERROR_MULTILINE
        const auto previous_line = _location.previous_line();
        const auto next_line     = _location.next_line();
        return absl::StrCat(ANSI_BOLD, _lexer.file_name(), ":", _location.line, ":",
                            _location.column, ANSI_RESET, ": ", ANSI_RED, "error: ", ANSI_RESET,
                            ANSI_BOLD, _msg, ANSI_RESET, "\n", previous_line.text(), source_line,
                            "\n", under_line, "\n", next_line.text(),
                            (next_line.text().empty() ? "" : "\n"));
#else
        return absl::StrCat(ANSI_BOLD, _lexer.file_name(), ":", _location.line, ":",
                            _location.column, ANSI_RESET, ": ", ANSI_RED, "error: ", ANSI_RESET,
                            ANSI_BOLD, _msg, ANSI_RESET, "\n", source_line, "\n", under_line, "\n");
#endif  // RAIN_PRINT_ERROR_MULTILINE
    }
};

}  // namespace rain::lang::err
