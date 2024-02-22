#pragma once

#include "absl/strings/str_cat.h"
#include "cirrus/lang/lexer.hpp"
#include "cirrus/lang/location.hpp"
#include "cirrus/util/colors.hpp"
#include "cirrus/util/result.hpp"

namespace cirrus::err {

class BinaryOperatorError : public util::Error {
    const lang::Lexer& _lexer;
    lang::Location     _lhs_location;
    lang::Location     _rhs_location;
    lang::Location     _op_location;
    std::string        _msg;

  public:
    BinaryOperatorError(const lang::Lexer& lexer, lang::Location lhs_location,
                        lang::Location rhs_location, lang::Location op_location, std::string msg)
        : _lexer(lexer),
          _lhs_location(lhs_location),
          _rhs_location(rhs_location),
          _op_location(op_location),
          _msg(std::move(msg)) {}
    ~BinaryOperatorError() override = default;

    [[nodiscard]] std::string message() const noexcept override {
        const auto  source_line = _op_location.whole_line().substr();
        std::string under_line;
        under_line.reserve(source_line.size() + ANSI_GREEN.size() + ANSI_RESET.size());
        under_line += ANSI_GREEN;

        int column = 1;
        if (_lhs_location.line() == _op_location.line()) {
            // Underline the LHS expression
            for (; column < _lhs_location.column(); ++column) {
                under_line += ' ';
            }

            for (int i = 0; i < _lhs_location.substr().size() - 1; ++i, ++column) {
                under_line += '~';
            }
        }

        {
            // Mark the operator with a caret
            for (; column < _op_location.column(); ++column) {
                under_line += ' ';
            }
            under_line += '^';
            ++column;
        }

        if (_rhs_location.line() == _op_location.line()) {
            // Underline the RHS expression
            for (; column < _rhs_location.column(); ++column) {
                under_line += ' ';
            }

            // ss << ANSI_GREEN;
            for (int i = 0; i < _rhs_location.substr().size() - 1; ++i, ++column) {
                under_line += '~';
            }
            under_line += ANSI_RESET;
        }

        return absl::StrCat(ANSI_BOLD, _lexer.file_name(), ":", _op_location.line(), ":",
                            _op_location.column(), ANSI_RESET, ": ", ANSI_RED,
                            "error: ", ANSI_RESET, ANSI_BOLD, _msg, ANSI_RESET, "\n", source_line,
                            "\n", under_line, "\n");
    }
};

}  // namespace cirrus::err
