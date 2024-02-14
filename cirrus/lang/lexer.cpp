#include "cirrus/lang/lexer.hpp"

#include <iostream>

#include "cirrus/lang/lexer+util.hpp"
#include "cirrus/lang/location.hpp"
#include "cirrus/lang/token.hpp"

namespace cirrus::lang {

Token Lexer::next() {
    {
        // Skip whitespace and comments
        const auto [it, line, column] = skip_whitespace(_source, _it, _line, _column);

        _it     = it;
        _line   = line;
        _column = column;

        if (_it == -1) [[unlikely]] {
            return Token{
                .kind     = TokenKind::Eof,
                .location = Location(_source, -1, -1, _line, _column),
            };
        }
    }

    // Save the start of the token
    const auto token_start = _it;
    const auto line        = _line;
    const auto column      = _column;

    char       c         = _source[_it];
    const auto next_char = [&, this]() -> char {
        ++_it;
        ++_column;
        c = _source[_it];
        return c;
    };

    // Integer or real
    if (std::isdigit(c)) {
        while (std::isdigit(c)) {
            next_char();
        }

        if (c != '.' || !std::isdigit(_source[_it + 1])) {
            return Token{
                .kind     = TokenKind::Integer,
                .location = Location(_source, token_start, _it, line, column),
            };
        }

        // Skip the decimal point
        next_char();

        while (std::isdigit(c)) {
            next_char();
        }

        return Token{
            .kind     = TokenKind::Real,
            .location = Location(_source, token_start, _it, line, column),
        };
    }

    // Identifier or keyword
    if (std::isalpha(c) || c == '_') {
        while (std::isalnum(c) || c == '_') {
            next_char();
        }

        const auto identifier = _source.substr(token_start, _it - token_start);
        const auto keyword    = find_keyword(identifier);
        if (keyword != TokenKind::Undefined) {
            return Token{
                .kind     = keyword,
                .location = Location(_source, token_start, _it, line, column),
            };
        }

        return Token{
            .kind     = TokenKind::Identifier,
            .location = Location(_source, token_start, _it, line, column),
        };
    }

    // Operator
    const auto [operator_kind, length] = find_operator(_source, _it);
    if (operator_kind != TokenKind::Undefined) {
        _it += length;
        _column += length;

        // std::cout << " Operator: " << std::string_view(token_start, _index - token_start)
        //           << " Kind: " << static_cast<int>(operator_kind) << " Length: " << length
        //           << std::endl;

        return Token{
            .kind     = operator_kind,
            .location = Location(_source, token_start, _it, line, column),
        };
    }

    return Token();
}

Token Lexer::peek() {
    // TODO: This can be implemented so much more efficiently
    // Currently it just calls next() and then rewinds the lexer

    const auto it     = _it;
    const auto line   = _line;
    const auto column = _column;

    const auto token = next();

    _it     = it;
    _line   = line;
    _column = column;

    return token;
}

}  // namespace cirrus::lang
