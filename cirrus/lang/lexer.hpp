#pragma once

#include <optional>
#include <string_view>
#include <vector>

#include "cirrus/lang/token.hpp"

namespace cirrus::lang {

class Lexer {
    std::string_view _source;
    std::string_view _file_name = "<unknown>";
    const char*      _index     = nullptr;
    int              _line      = 1;
    int              _column    = 1;

  public:
    explicit Lexer(std::string_view source) : _source(source), _index(&_source[0]) {}
    Lexer(std::string_view file_name, std::string_view source)
        : _source(source), _file_name(file_name), _index(&_source[0]) {}

    [[nodiscard]] std::string_view file_name() const noexcept { return _file_name; }
    [[nodiscard]] Location         get_whole_line(Location location) const noexcept;

    Token next();
    Token peek();
};

}  // namespace cirrus::lang
