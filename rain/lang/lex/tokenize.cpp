#include "rain/lang/lex/tokenize.hpp"

#include <algorithm>
#include <array>
#include <cctype>
#include <tuple>

#include "rain/lang/lex/token.hpp"

namespace {

// Check the iterator position compared to BOTH the start AND the end of the string_view.
constexpr bool contains(const std::string_view s, const char* it) {
    return it >= &*s.begin() && it < &*s.end();
}

// Check the iterator position compared ONLY the end of the string_view.
constexpr bool past_end(const std::string_view s, const char* it) { return it >= &*s.end(); }

}  // namespace

namespace rain::lang::lex {

[[nodiscard]] State skip_whitespace(State state) {
    if (state.it == nullptr || !contains(state.source, state.it)) [[unlikely]] {
        return State{nullptr, state.line, state.column};
    }

    char c = *state.it;
    do {
        // Skip whitespace
        while (std::isspace(c)) {
            if (c == '\n') {
                ++state.line;
                state.column = 1;
            } else {
                ++state.column;
            }

            ++state.it;
            if (past_end(state.source, state.it)) [[unlikely]] {
                return State{nullptr, state.line, state.column};
            }
            c = *state.it;
        }

        // Skip comments
        if (c == '/' && !past_end(state.source, state.it + 1) && state.it[1] == '/') {
            while (c != '\n' && c != '\0') {
                ++state.column;
                ++state.it;
                if (past_end(state.source, state.it)) [[unlikely]] {
                    return State{nullptr, state.line, state.column};
                }
                c = *state.it;
            }
        }
    } while (std::isspace(c));

    return state;
}

[[nodiscard]] TokenKind find_keyword(std::string_view word) {
    constexpr std::array<std::tuple<std::string_view, TokenKind>, 11> KEYWORDS{
        // clang-format off
        // <keep-sorted>
        std::make_tuple("else", TokenKind::Else),
        std::make_tuple("export", TokenKind::Export),
        std::make_tuple("false", TokenKind::False),
        std::make_tuple("fn", TokenKind::Fn),
        std::make_tuple("if", TokenKind::If),
        std::make_tuple("let", TokenKind::Let),
        std::make_tuple("return", TokenKind::Return),
        std::make_tuple("self", TokenKind::Self),
        std::make_tuple("struct", TokenKind::Struct),
        std::make_tuple("true", TokenKind::True),
        std::make_tuple("while", TokenKind::While),
        // </keep-sorted>
        // clang-format on
    };

    // Binary search through the list of keywords (this is why the list MUST BE sorted!).
    const auto it =
        std::lower_bound(KEYWORDS.begin(), KEYWORDS.end(), word,
                         [](const auto& lhs, const auto& rhs) { return std::get<0>(lhs) < rhs; });
    if (it != KEYWORDS.end() && std::get<0>(*it) == word) {
        return std::get<1>(*it);
    }

    return TokenKind::Undefined;
}

[[nodiscard]] TokenKind find_operator(State state) {
    static constexpr std::array<TokenKind, 128> OPERATORS = []() {
        std::array<TokenKind, 128> operators{};

        operators['#'] = TokenKind::Hash;
        operators['.'] = TokenKind::Period;
        operators[','] = TokenKind::Comma;
        operators[':'] = TokenKind::Colon;
        operators[';'] = TokenKind::Semicolon;

        operators['@'] = TokenKind::At;
        operators['!'] = TokenKind::Exclaim;
        operators['&'] = TokenKind::Ampersand;
        operators['|'] = TokenKind::Pipe;
        operators['^'] = TokenKind::Caret;
        operators['+'] = TokenKind::Plus;
        operators['-'] = TokenKind::Minus;
        operators['*'] = TokenKind::Star;
        operators['/'] = TokenKind::Slash;
        operators['%'] = TokenKind::Percent;

        operators['='] = TokenKind::Equal;
        operators['<'] = TokenKind::Less;
        operators['>'] = TokenKind::Greater;

        operators['('] = TokenKind::LRoundBracket;
        operators[')'] = TokenKind::RRoundBracket;
        operators['['] = TokenKind::LSquareBracket;
        operators[']'] = TokenKind::RSquareBracket;
        operators['{'] = TokenKind::LCurlyBracket;
        operators['}'] = TokenKind::RCurlyBracket;

        return operators;
    }();

    const auto op = OPERATORS[static_cast<uint8_t>(*state.it)];
    if (past_end(state.source, state.it + 1)) [[unlikely]] {
        return op;
    }

    switch (op) {
        case TokenKind::Minus:
            switch (state.it[1]) {
                case '>':
                    return TokenKind::RArrow;
                default:
                    break;
            }
            break;

        case TokenKind::Equal:
            switch (state.it[1]) {
                case '=':
                    return TokenKind::EqualEqual;
                default:
                    break;
            }
            break;

        case TokenKind::Exclaim:
            switch (state.it[1]) {
                case '=':
                    return TokenKind::ExclaimEqual;
                default:
                    break;
            }

        case TokenKind::Less:
            switch (state.it[1]) {
                case '=':
                    return TokenKind::LessEqual;
                case '<':
                    return TokenKind::LessLess;
                default:
                    break;
            }

        case TokenKind::Greater:
            switch (state.it[1]) {
                case '=':
                    return TokenKind::GreaterEqual;
                case '>':
                    return TokenKind::GreaterGreater;
                default:
                    break;
            }

        default:
            break;
    }

    return op;
}

[[nodiscard]] int operator_length(TokenKind kind) {
    static constexpr std::array<int, TokenKind::Count> OPERATOR_LENGTH = []() {
        std::array<int, TokenKind::Count> lengths{};

        lengths[TokenKind::Hash]      = 1;
        lengths[TokenKind::Period]    = 1;
        lengths[TokenKind::Comma]     = 1;
        lengths[TokenKind::Colon]     = 1;
        lengths[TokenKind::Semicolon] = 1;

        lengths[TokenKind::At]        = 1;
        lengths[TokenKind::Exclaim]   = 1;
        lengths[TokenKind::Ampersand] = 1;
        lengths[TokenKind::Pipe]      = 1;
        lengths[TokenKind::Caret]     = 1;
        lengths[TokenKind::Plus]      = 1;
        lengths[TokenKind::Minus]     = 1;
        lengths[TokenKind::Star]      = 1;
        lengths[TokenKind::Slash]     = 1;
        lengths[TokenKind::Percent]   = 1;

        lengths[TokenKind::Equal]   = 1;
        lengths[TokenKind::Less]    = 1;
        lengths[TokenKind::Greater] = 1;

        lengths[TokenKind::LRoundBracket]  = 1;
        lengths[TokenKind::RRoundBracket]  = 1;
        lengths[TokenKind::LSquareBracket] = 1;
        lengths[TokenKind::RSquareBracket] = 1;
        lengths[TokenKind::LCurlyBracket]  = 1;
        lengths[TokenKind::RCurlyBracket]  = 1;

        lengths[TokenKind::RArrow]         = 2;
        lengths[TokenKind::LessLess]       = 2;
        lengths[TokenKind::GreaterGreater] = 2;
        lengths[TokenKind::EqualEqual]     = 2;
        lengths[TokenKind::ExclaimEqual]   = 2;
        lengths[TokenKind::LessEqual]      = 2;
        lengths[TokenKind::GreaterEqual]   = 2;

        return lengths;
    }();

    return OPERATOR_LENGTH[kind];
}

[[nodiscard]] std::tuple<Token, State> next_token(State state) {
    {
        // Skip whitespace and comments
        state = skip_whitespace(state);

        if (state.it == nullptr) [[unlikely]] {
            return std::make_tuple(
                Token{
                    .kind     = TokenKind::EndOfFile,
                    .location = Location(state.file_name, state.source, state.source.end(),
                                         state.source.end(), state.line, state.column),
                },
                state);
        }
    }

    // Save the start of the token
    const auto start_it     = state.it;
    const auto start_line   = state.line;
    const auto start_column = state.column;

    char       c         = *state.it;
    const auto next_char = [&]() -> char {
        ++state.column;
        ++state.it;
        if (past_end(state.source, state.it)) [[unlikely]] {
            c = '\0';
            return '\0';
        }
        c = *state.it;
        return c;
    };

    // Integer or float
    if (std::isdigit(c)) {
        do {
            next_char();
        } while (std::isdigit(c));

        if (c != '.' || !std::isdigit(state.it[1])) {
            return std::make_tuple(
                Token{
                    .kind     = TokenKind::Integer,
                    .location = Location(state.file_name, state.source, start_it, state.it,
                                         start_line, start_column),
                },
                state);
        }

        // Skip the decimal point
        next_char();

        while (std::isdigit(c)) {
            next_char();
        }

        return std::make_tuple(
            Token{
                .kind     = TokenKind::Float,
                .location = Location(state.file_name, state.source, start_it, state.it, start_line,
                                     start_column),
            },
            state);
    }

    // Identifier or keyword
    if (std::isalpha(c) || c == '_') {
        while (std::isalnum(c) || c == '_') {
            next_char();
        }

        const auto identifier = std::string_view{start_it, state.it};
        const auto keyword    = find_keyword(identifier);
        if (keyword != TokenKind::Undefined) {
            return std::make_tuple(
                Token{
                    .kind     = keyword,
                    .location = Location(state.file_name, state.source, start_it, state.it,
                                         start_line, start_column),
                },
                state);
        }

        return std::make_tuple(
            Token{
                .kind     = TokenKind::Identifier,
                .location = Location(state.file_name, state.source, start_it, state.it, start_line,
                                     start_column),
            },
            state);
    }

    // Operator
    const auto operator_kind = find_operator(state);
    if (operator_kind != TokenKind::Undefined) {
        const auto length = operator_length(operator_kind);
        state.it += length;
        state.column += length;

        return std::make_tuple(
            Token{
                .kind     = operator_kind,
                .location = Location(state.file_name, state.source, start_it, state.it, start_line,
                                     start_column),
            },
            state);
    }

    // Unknown token
    return std::make_tuple(
        Token{
            .kind     = TokenKind::Undefined,
            .location = Location(state.file_name, state.source, start_it, state.it, start_line,
                                 start_column),
        },
        state);
}

}  // namespace rain::lang::lex
