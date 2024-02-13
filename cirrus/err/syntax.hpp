#pragma once

#include <sstream>

#include "cirrus/lang/lexer.hpp"
#include "cirrus/lang/location.hpp"
#include "cirrus/util/colors.hpp"
#include "cirrus/util/result.hpp"

namespace cirrus::err {

class SyntaxError : public util::Error {
    const lang::Lexer& _lexer;
    lang::Location     _location;
    std::string        _msg;

  public:
    SyntaxError(const lang::Lexer& lexer, lang::Location location, std::string msg)
        : _lexer(lexer), _location(location), _msg(std::move(msg)) {}
    ~SyntaxError() override = default;

    [[nodiscard]] std::string message() const noexcept override {
        std::stringstream ss;

        ss << COUT_COLOR_BOLD(_lexer.file_name()
                              << ':' << _location.line() << ':' << _location.column() << ": ")
           << COUT_COLOR_RED("error: ") << COUT_COLOR_BOLD(_msg) << '\n';
        ss << _lexer.get_whole_line(_location).substr() << '\n';

        for (int i = 0; i < _location.column() - 1; ++i) {
            ss << ' ';
        }
        ss << ANSI_GREEN << '^';
        for (int i = 0; i < _location.substr().size() - 1; ++i) {
            ss << '~';
        }
        ss << ANSI_RESET << '\n';

        return ss.str();
    }
};

}  // namespace cirrus::err
