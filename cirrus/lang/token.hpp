#pragma once

#include <cstdint>
#include <string_view>

#include "cirrus/lang/location.hpp"

namespace cirrus::lang {

enum TokenKind : uint8_t {
    Undefined,
    Eof,

    // Atoms and literals
    Identifier,
    Integer,
    Real,

    // Keywords
    True,
    False,
    If,
    Else,
    Loop,
    While,
    For,
    Struct,

    // Operators
    Hash,
    Period,
    Comma,
    Colon,
    Semicolon,

    Plus,
    Minus,
    Star,
    Slash,
    Percent,

    // Equal,
    // NotEqual,
    // LessThan,
    // LessThanOrEqual,
    // GreaterThan,
    // GreaterThanOrEqual,

    LRoundBracket,
    RRoundBracket,
    LSquareBracket,
    RSquareBracket,
    LCurlyBracket,
    RCurlyBracket,
};

struct Token {
    TokenKind kind = TokenKind::Undefined;
    Location  location;
};

}  // namespace cirrus::lang
