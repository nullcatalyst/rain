#pragma once

#include <string>

#include "absl/strings/str_cat.h"
#include "rain/lang/lexer.hpp"
#include "rain/lang/location.hpp"
#include "rain/util/colors.hpp"
#include "rain/util/result.hpp"

namespace rain::err {

class SyntaxError : public util::Error {
    const lang::Lexer& _lexer;
    lang::Location     _location;
    std::string        _msg;

  public:
    SyntaxError(const lang::Lexer& lexer, lang::Location location, std::string msg)
        : _lexer(lexer), _location(location), _msg(std::move(msg)) {}
    ~SyntaxError() override = default;

    [[nodiscard]] std::string message() const noexcept override {
        const auto  source_line = _location.whole_line().substr();
        std::string under_line;
        under_line.reserve(source_line.size() + ANSI_GREEN.size() + ANSI_RESET.size());

        under_line += ANSI_GREEN;
        for (int i = 0; i < _location.column() - 1; ++i) {
            under_line += ' ';
        }
        under_line += '^';
        for (int i = 0; i < static_cast<int>(_location.substr().size()) - 1; ++i) {
            under_line += '~';
        }
        under_line += ANSI_RESET;

        return absl::StrCat(ANSI_BOLD, _lexer.file_name(), ":", _location.line(), ":",
                            _location.column(), ANSI_RESET, ": ", ANSI_RED, "error: ", ANSI_RESET,
                            ANSI_BOLD, _msg, ANSI_RESET, "\n", source_line, "\n", under_line, "\n");
    }
};

}  // namespace rain::err
