#pragma once

#include "rain/lang/lex/state.hpp"
#include "rain/lang/lex/token.hpp"

namespace rain::lang::lex {

class Lexer {
  public:
    virtual ~Lexer() = default;

    [[nodiscard]] virtual std::string_view source() const noexcept    = 0;
    [[nodiscard]] virtual std::string_view file_name() const noexcept = 0;

    /** Save the current state of the lexer, so that it can be restored later. */
    [[nodiscard]] virtual State save_state() const noexcept = 0;

    /**
     * Restore the state of lexer to a previous state.
     *
     * Passing in a state that was not returned by `save_state` of the same instance is undefined
     * behavior.
     */
    virtual void restore_state(State state) noexcept = 0;

    virtual Token next() = 0;
    virtual Token peek() = 0;
};

}  // namespace rain::lang::lex
