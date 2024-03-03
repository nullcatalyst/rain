#pragma once

#include <cstdint>
#include <string_view>

#include "rain/lang/location.hpp"

namespace rain::lang {

enum TokenKind : uint32_t {
    Undefined,
    Eof,

    // Atoms and literals
    Identifier,
    Integer,
    Float,

    // Keywords
    True,
    False,
    If,
    Else,
    Loop,
    While,
    For,
    Return,
    Break,
    Continue,
    Export,
    Struct,
    Interface,
    Fn,
    Let,

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
    LessLess,
    GreaterGreater,

    Equal,
    EqualEqual,
    ExclaimEqual,
    Less,
    LessEqual,
    Greater,
    GreaterEqual,

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

}  // namespace rain::lang
