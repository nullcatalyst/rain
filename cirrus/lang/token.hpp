#pragma once

#include <cstdint>
#include <string_view>

#include "cirrus/lang/location.hpp"

namespace cirrus::lang {

enum TokenKind : uint32_t {
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
    Fn,

    // Operators
    Hash,
    Period,
    Comma,
    Colon,
    Semicolon,
    RArrow,

    At,
    Exclaim,
    Ampersand,
    Pipe,
    Caret,
    Plus,
    Minus,
    Star,
    Slash,
    Percent,
    LessThanLessThan,
    GreaterThanGreaterThan,

    Equal,
    EqualEqual,
    ExclaimEqual,
    LessThan,
    LessThanEqual,
    GreaterThan,
    GreaterThanEqual,

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
