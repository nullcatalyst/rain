#include "rain/lang/err/unary_operator.hpp"

#include <cmath>

#include "absl/strings/str_cat.h"
#include "rain/util/colors.hpp"

namespace rain::lang::err {

std::string UnaryOperatorError::message() const noexcept {
    const auto  source_line = _op_location.whole_line().text();
    int         capacity    = source_line.size();
    std::string under_line;
    under_line.reserve(capacity);

    // Column indexing starts at 1, whereas capacity is indexed from 0.
    // Offset by 1 to make them match.
    capacity += 1;

    int column = 1;
    if (_expression_location.line == _op_location.line &&
        _expression_location.column < _op_location.column) {
        // The operator is a postfix operator, so underline the expression first.
        // Underline the LHS expression.

        for (int end = std::min(capacity, _expression_location.column); column < end; ++column) {
            under_line += ' ';
        }

        for (int end = std::min(capacity, _expression_location.column +
                                              static_cast<int>(_expression_location.text().size()));
             column < end; ++column) {
            under_line += '~';
        }
    }

    {  // Underline the operator with carets.
        for (int end = std::min(capacity, _op_location.column); column < end; ++column) {
            under_line += ' ';
        }

        for (int end = std::min(capacity,
                                _op_location.column + static_cast<int>(_op_location.text().size()));
             column < end; ++column) {
            under_line += '^';
        }
    }

    if (_expression_location.line == _op_location.line &&
        _expression_location.column > _op_location.column) {
        // The operator is a prefix operator, so underline the expression last.
        // Underline the RHS expression.

        for (int end = std::min(capacity, _expression_location.column); column < end; ++column) {
            under_line += ' ';
        }

        for (int end = std::min(capacity, _expression_location.column +
                                              static_cast<int>(_expression_location.text().size()));
             column < end; ++column) {
            under_line += '~';
        }
    }

    return absl::StrCat(ANSI_BOLD, _op_location.file_name, ":", _op_location.line, ":",
                        _op_location.column, ANSI_RESET, ": ", ANSI_RED, "error: ", ANSI_RESET,
                        ANSI_BOLD, _msg, ANSI_RESET, "\n", source_line, "\n", ANSI_GREEN,
                        under_line, ANSI_RESET, "\n");
}

}  // namespace rain::lang::err
