#pragma once

#include "rain/util/result.hpp"
#include "rain/util/unreachable.hpp"

namespace rain::lang::parse {

template <typename ItemFn /* util::Result<void>(lex::Lexer&) */,
          typename MissingSepFn /* util::Result<void>(lex::Lexer&, lex::Token) */>
util::Result<void> parse_list(lex::Lexer& lexer, lex::TokenKind separator, lex::TokenKind end,
                              ItemFn&& parse_item, MissingSepFn&& missing_sep) {
    for (;;) {
        if (const auto token = lexer.peek(); token.kind == end) {
            // Do NOT consume the end token. The caller will consume it.
            // This is in order for the end Location to be tracked correctly.
            return {};
        }

        {
            auto result = parse_item(lexer);
            FORWARD_ERROR(result);
        }

        auto end_or_sep_token = lexer.peek();
        if (end_or_sep_token.kind == end) {
            // Do NOT consume the end token. The caller will consume it.
            // This is in order for the end Location to be tracked correctly.
            return {};
        }

        if (end_or_sep_token.kind == separator) {
            // Note that a trailing separator is allowed. The next iteration will check for the end
            // token.
            lexer.next();  // Consume the separator token
        } else {
            auto result = missing_sep(lexer, end_or_sep_token);
            FORWARD_ERROR(result);
        }
    }

    util::unreachable();
}

template <typename ItemFn /* util::Result<void>(lex::Lexer&) */>
util::Result<void> parse_many(lex::Lexer& lexer, lex::TokenKind end, ItemFn&& parse_item) {
    for (;;) {
        if (const auto token = lexer.peek(); token.kind == end) {
            // Do NOT consume the end token. The caller will consume it.
            // This is in order for the end Location to be tracked correctly.
            return {};
        }

        {
            auto result = parse_item(lexer);
            FORWARD_ERROR(result);
        }
    }

    util::unreachable();
}

}  // namespace rain::lang::parse
