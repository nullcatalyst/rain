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

    class LexerState {
        const Lexer&         _lexer;
        util::StringIterator _it;
        int                  _line;
        int                  _column;

        friend class Lexer;

      public:
        explicit LexerState(const Lexer& lexer)
            : _lexer(lexer), _it(lexer._it), _line(lexer._line), _column(lexer._column) {}
    };

    LexerState save_state() const { return LexerState(*this); }

    void restore_state(LexerState state) {
        assert(this == &state._lexer);
        _it     = state._it;
        _line   = state._line;
        _column = state._column;
    }

    Token next();

    Token peek() {
        auto state = save_state();
        auto token = next();
        restore_state(state);
        return token;
    }
};

}  // namespace rain::lang
