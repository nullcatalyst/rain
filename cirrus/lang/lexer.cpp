#include "cirrus/lang/lexer.hpp"

#include <iostream>

#include "cirrus/lang/lexer+util.hpp"
#include "cirrus/lang/location.hpp"
#include "cirrus/lang/token.hpp"

namespace cirrus::lang {

Location Lexer::get_whole_line(Location location) const noexcept {
    const char* line_start = location.source.begin();

    if (line_start == _source.end()) [[unlikely]] {
        // The source location is at the end of the file

        if (_source.empty()) [[unlikely]] {
            // This can happen if the source is empty
            return location;
        }

        --line_start;
    }

    while (line_start > _source.begin() && line_start[-1] != '\n') {
        --line_start;
        --location.column;
    }

    const char* line_end = location.source.end();
    while (line_end < _source.end() && *line_end != '\n') {
        ++line_end;
    }

    location.source = std::string_view(line_start, line_end - line_start);
    return location;
}

Token Lexer::next() {
    {
        // Skip whitespace and comments
        const auto [index, line, column] = skip_whitespace(_index, _line, _column);

        _index  = index;
        _line   = line;
        _column = column;

        if (_index == nullptr) [[unlikely]] {
            return Token{
                .kind = TokenKind::Eof,
                .location =
                    Location{
                        .line   = _line,
                        .column = _column,
                        .source = std::string_view(_index, 0),
                    },
            };
        }
    }

    // Save the start of the token
    const auto token_start = _index;
    const auto line        = _line;
    const auto column      = _column;

    // Integer or real
    if (std::isdigit(*_index)) {
        while (std::isdigit(*_index)) {
            ++_index;
            ++_column;
        }

        if (*_index != '.' || !std::isdigit(*(_index + 1))) {
            return Token{
                .kind = TokenKind::Integer,
                .location =
                    Location{
                        .line   = line,
                        .column = column,
                        .source = std::string_view(token_start, _index - token_start),
                    },
            };
        }

        // Skip the decimal point
        ++_index;
        ++_column;

        while (std::isdigit(*_index)) {
            ++_index;
            ++_column;
        }

        return Token{
            .kind = TokenKind::Real,
            .location =
                Location{
                    .line   = line,
                    .column = column,
                    .source = std::string_view(token_start, _index - token_start),
                },
        };
    }

    // Identifier or keyword
    if (std::isalpha(*_index) || *_index == '_') {
        while (std::isalnum(*_index) || *_index == '_') {
            ++_index;
            ++_column;
        }

        const auto identifier = std::string_view(token_start, _index - token_start);

        const auto keyword = find_keyword(identifier);
        if (keyword != TokenKind::Undefined) {
            return Token{
                .kind = keyword,
                .location =
                    Location{
                        .line   = line,
                        .column = column,
                        .source = identifier,
                    },
            };
        }

        return Token{
            .kind = TokenKind::Identifier,
            .location =
                Location{
                    .line   = line,
                    .column = column,
                    .source = identifier,
                },
        };
    }

    // Operator
    const auto [operator_kind, length] = find_operator(_index);
    if (operator_kind != TokenKind::Undefined) {
        _index += length;
        _column += length;

        // std::cout << " Operator: " << std::string_view(token_start, _index - token_start)
        //           << " Kind: " << static_cast<int>(operator_kind) << " Length: " << length
        //           << std::endl;

        return Token{
            .kind = operator_kind,
            .location =
                Location{
                    .line   = line,
                    .column = column,
                    .source = std::string_view(token_start, _index - token_start),
                },
        };
    }

    return Token();
}

Token Lexer::peek() {
    // TODO: This can be implemented so much more efficiently
    // Currently it just calls next() and then rewinds the lexer

    const auto index  = _index;
    const auto line   = _line;
    const auto column = _column;

    const auto token = next();

    _index  = index;
    _line   = line;
    _column = column;

    return token;
}

}  // namespace cirrus::lang
