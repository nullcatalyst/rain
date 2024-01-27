#pragma once

#include <sstream>

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
        std::stringstream ss;

        ss << ANSI_BOLD << _lexer.file_name() << ":" << _op_location.line << ":"
           << _op_location.column << ": " << ANSI_RED << "error: " << ANSI_RESET << ANSI_BOLD
           << _msg << ANSI_RESET << "\n";
        ss << _lexer.get_whole_line(_op_location).source << "\n";

        int column = 1;
        if (_lhs_location.line == _op_location.line) {
            // Underline the LHS expression
            for (; column < _lhs_location.column; ++column) {
                ss << ' ';
            }

            ss << ANSI_GREEN;
            for (int i = 0; i < _lhs_location.source.size() - 1; ++i, ++column) {
                ss << '~';
            }
            ss << ANSI_RESET;
        }

        {
            // Mark the operator with a caret
            for (; column < _op_location.column; ++column) {
                ss << ' ';
            }
            ss << ANSI_GREEN << '^' << ANSI_RESET;
            ++column;
        }

        if (_rhs_location.line == _op_location.line) {
            // Underline the RHS expression
            for (; column < _rhs_location.column; ++column) {
                ss << ' ';
            }

            ss << ANSI_GREEN;
            for (int i = 0; i < _rhs_location.source.size() - 1; ++i, ++column) {
                ss << '~';
            }
            ss << ANSI_RESET << '\n';
        }

        return ss.str();
    }
};

}  // namespace cirrus::err
