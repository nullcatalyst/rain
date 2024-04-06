#pragma once

#include <cstdint>
#include <string_view>

#include "rain/lang/lex/location.hpp"

namespace rain::lang::lex {

enum TokenKind : uint32_t {
    Undefined,
    EndOfFile,

    // Atoms and literals
    Identifier,
    Integer,
    Float,
    String,

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
    Defer,
    Export,
    Extern,
    Struct,
    Interface,
    Impl,
    Self,
    Null,
    Fn,
    Let,

    // Operators
    Hash,
    Period,
    Comma,
    Colon,
    Semicolon,
    RArrow,
    Question,

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

    // Yes, we all agree that these are bad names for these tokens, that there are "more correct"
    // names, but can we also agree that we all know (unamibiguously) what these names refer to?
    LRoundBracket,
    RRoundBracket,
    LSquareBracket,
    RSquareBracket,
    LCurlyBracket,
    RCurlyBracket,

    Count,
};

struct Token {
    TokenKind kind = TokenKind::Undefined;
    Location  location;

    /**
     * Denotes whether there was whitespace between this token and the one before it.
     *
     * It is useful for disambiguating some parsings of the code.
     * eg:
     *      do_something(1, 2)
     *      (3, 4) // <- Is this a function call or a parenthesized tuple expression?
     */
    bool whitespace_before = false;

    constexpr std::string_view text() const { return location.text(); }
};

}  // namespace rain::lang::lex
