#include "rain/lang/lexer.hpp"

#include "rain/lang/lexer+util.hpp"
#include "rain/lang/location.hpp"
#include "rain/lang/token.hpp"

namespace rain::lang {

void Lexer::_tokenize() {
    {
        // Skip whitespace and comments
        const auto [it, line, column] = skip_whitespace(_source, _it, _line, _column);
        _it                           = it;
        _line                         = line;
        _column                       = column;

        if (_it == nullptr) [[unlikely]] {
            _tokens.emplace_back(Token{
                .kind     = TokenKind::Eof,
                .location = Location(_source, _source.end(), _source.end(), _line, _column),
            });
            return;
        }
    }

    // Save the start of the token
    const auto token_start = _it;
    const auto line        = _line;
    const auto column      = _column;

    char       c         = *_it;
    const auto next_char = [&, this]() -> char {
        ++_it;
        ++_column;
        c = *_it;
        return c;
    };

    // Integer or float
    if (std::isdigit(c)) {
        while (std::isdigit(c)) {
            next_char();
        }

        if (c != '.' || !std::isdigit(_it[1])) {
            _tokens.emplace_back(Token{
                .kind     = TokenKind::Integer,
                .location = Location(_source, token_start, _it, line, column),
            });
            return;
        }

        // Skip the decimal point
        next_char();

        while (std::isdigit(c)) {
            next_char();
        }

        _tokens.emplace_back(Token{
            .kind     = TokenKind::Float,
            .location = Location(_source, token_start, _it, line, column),
        });
        return;
    }

    // Identifier or keyword
    if (std::isalpha(c) || c == '_') {
        while (std::isalnum(c) || c == '_') {
            next_char();
        }

        const auto identifier = std::string_view{token_start, _it};
        const auto keyword    = find_keyword(identifier);
        if (keyword != TokenKind::Undefined) {
            _tokens.emplace_back(Token{
                .kind     = keyword,
                .location = Location(_source, token_start, _it, line, column),
            });
            return;
        }

        _tokens.emplace_back(Token{
            .kind     = TokenKind::Identifier,
            .location = Location(_source, token_start, _it, line, column),
        });
        return;
    }

    // Operator
    const auto [operator_kind, length] = find_operator(_source, _it);
    if (operator_kind != TokenKind::Undefined) {
        _it += length;
        _column += length;

        _tokens.emplace_back(Token{
            .kind     = operator_kind,
            .location = Location(_source, token_start, _it, line, column),
        });
        return;
    }
}

}  // namespace rain::lang
