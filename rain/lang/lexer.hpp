#pragma once

#include <optional>
#include <string_view>
#include <vector>

#include "rain/lang/token.hpp"
#include "rain/util/string.hpp"

namespace rain::lang {

class Lexer {
    util::String         _source;
    util::String         _file_name = "<unknown>";
    util::StringIterator _it        = 0;
    int                  _line      = 1;
    int                  _column    = 1;

  public:
    explicit Lexer(util::String source) : _source(std::move(source)) {}
    Lexer(util::String file_name, util::String source)
        : _source(std::move(source)), _file_name(file_name) {}

    [[nodiscard]] constexpr const util::String& file_name() const noexcept { return _file_name; }

    Token next();
    Token peek();
};

}  // namespace rain::lang
