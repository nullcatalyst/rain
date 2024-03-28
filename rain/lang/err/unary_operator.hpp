#pragma once

#include <string>

#include "absl/strings/str_cat.h"
#include "rain/lang/lex/lexer.hpp"
#include "rain/lang/lex/location.hpp"
#include "rain/util/colors.hpp"
#include "rain/util/result.hpp"

namespace rain::lang::err {

class UnaryOperatorError : public util::Error {
    lex::Location _expression_location;
    lex::Location _op_location;
    std::string   _msg;

  public:
    UnaryOperatorError(lex::Location expression_location, lex::Location op_location,
                       std::string msg)
        : _expression_location(expression_location),
          _op_location(op_location),
          _msg(std::move(msg)) {}
    ~UnaryOperatorError() override = default;

    [[nodiscard]] std::string message() const noexcept override {
        const auto  source_line = _op_location.whole_line().text();
        std::string under_line;
        under_line.reserve(source_line.size() + ANSI_GREEN.size() + ANSI_RESET.size());
        under_line += ANSI_GREEN;

        int column = 1;
        if (_expression_location.line == _op_location.line &&
            _expression_location.column < _op_location.column) {
            // Underline the LHS expression
            for (; column < _expression_location.column; ++column) {
                under_line += ' ';
            }

            for (int i = 0; i < _expression_location.text().size() - 1; ++i, ++column) {
                under_line += '~';
            }
        }

        {
            // Mark the operator with a caret
            for (; column < _op_location.column; ++column) {
                under_line += ' ';
            }
            under_line += '^';
            ++column;
        }

        if (_expression_location.line == _op_location.line &&
            _op_location.column < _expression_location.column) {
            // Underline the RHS expression
            for (; column < _expression_location.column; ++column) {
                under_line += ' ';
            }

            // ss << ANSI_GREEN;
            for (int i = 0; i < _expression_location.text().size() - 1; ++i, ++column) {
                under_line += '~';
            }
            under_line += ANSI_RESET;
        }

        return absl::StrCat(ANSI_BOLD, _op_location.file_name, ":", _op_location.line, ":",
                            _op_location.column, ANSI_RESET, ": ", ANSI_RED, "error: ", ANSI_RESET,
                            ANSI_BOLD, _msg, ANSI_RESET, "\n", source_line, "\n", under_line, "\n");
    }
};

}  // namespace rain::lang::err
