#pragma once

#include <algorithm>
#include <array>
#include <tuple>

#include "cirrus/lang/token.hpp"

namespace cirrus::lang {

namespace {

constexpr std::array<std::tuple<std::string_view, TokenKind>, 5> KEYWORDS{
    // Keep this sorted alphabetically
    // clang-format off
    std::make_tuple("else", TokenKind::Else),
    std::make_tuple("false", TokenKind::False),
    std::make_tuple("if", TokenKind::If),
    std::make_tuple("struct", TokenKind::Struct),
    std::make_tuple("true", TokenKind::True),
    // clang-format on
};

TokenKind find_keyword(std::string_view source) {
    // Binary search through the list of keywords
    const auto it =
        std::lower_bound(KEYWORDS.begin(), KEYWORDS.end(), source,
                         [](const auto& lhs, const auto& rhs) { return std::get<0>(lhs) < rhs; });
    if (it != KEYWORDS.end() && std::get<0>(*it) == source) {
        return std::get<1>(*it);
    }

    return TokenKind::Undefined;
}

constexpr std::array<TokenKind, 128> OPERATORS = []() {
    std::array<TokenKind, 128> operators{};

    operators['#'] = TokenKind::Hash;
    operators['.'] = TokenKind::Period;
    operators[','] = TokenKind::Comma;
    operators[':'] = TokenKind::Colon;
    operators[';'] = TokenKind::Semicolon;

    operators['+'] = TokenKind::Plus;
    operators['-'] = TokenKind::Minus;
    operators['*'] = TokenKind::Star;
    operators['/'] = TokenKind::Slash;
    operators['%'] = TokenKind::Percent;

    operators['('] = TokenKind::LRoundBracket;
    operators[')'] = TokenKind::RRoundBracket;
    operators['['] = TokenKind::LSquareBracket;
    operators[']'] = TokenKind::RSquareBracket;
    operators['{'] = TokenKind::LCurlyBracket;
    operators['}'] = TokenKind::RCurlyBracket;

    return operators;
}();

TokenKind find_operator(const char* index) {
    if (index == nullptr || *index < 0) [[unlikely]] {
        return TokenKind::Undefined;
    }
    return OPERATORS[static_cast<uint8_t>(*index)];
}

std::tuple<const char* /*index*/, int /*line*/, int /*column*/> skip_whitespace(const char* index,
                                                                                int         line,
                                                                                int column) {
    if (index == nullptr) [[unlikely]] {
        return std::make_tuple(nullptr, line, column);
    }

    do {
        // Skip whitespace
        while (std::isspace(*index)) {
            if (*index == '\n') {
                ++line;
                column = 1;
            } else {
                ++column;
            }

            ++index;
        }

        // Skip comments
        if (*index == '/' && *(index + 1) == '/') {
            while (*index != '\n' && *index != '\0') {
                ++index;
                ++column;
            }
        }
    } while (std::isspace(*index));

    // Reset the index to the nullptr if we've reached the end of the string
    if (*index == '\0') [[unlikely]] {
        index = nullptr;
    }

    return std::make_tuple(index, line, column);
}

}  // namespace

}  // namespace cirrus::lang
