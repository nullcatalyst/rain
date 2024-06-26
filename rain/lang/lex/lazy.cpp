#include "rain/lang/lex/lazy.hpp"

#include "rain/lang/lex/location.hpp"
#include "rain/lang/lex/token.hpp"
#include "rain/lang/lex/tokenize.hpp"

namespace rain::lang::lex {

Token LazyLexer::next() {
    auto [token, state] = next_token(_state);
    _state              = state;
    return token;
}

Token LazyLexer::peek() {
    auto [token, state] = next_token(_state);
    return token;
}

}  // namespace rain::lang::lex
