#include "cirrus/lang/lexer.hpp"

#include <gtest/gtest.h>

#include "cirrus/lang/lexer+util.hpp"

using namespace cirrus::lang;

TEST(Lexer, get_line) {
    {
        const std::string_view source = "Hello, world!";
        Lexer                  lexer(source);
        const auto             location = Location{
                        .line   = 1,
                        .column = 1,
                        .source = source.substr(0, 5),
        };
        const auto result = lexer.get_whole_line(location);
        EXPECT_EQ(result.line, 1);
        EXPECT_EQ(result.column, 1);
        EXPECT_EQ(result.source, source);
    }

    {
        const std::string_view source =
            "one\ntwo\nthree\nfour\nfive\nsix\nseven\neight\nnine\nten\n";
        Lexer      lexer(source);
        const auto location = Location{
            .line   = 4,
            .column = 2,
            // "ou" from "four"
            .source = source.substr(15, 2),
        };
        const auto result = lexer.get_whole_line(location);
        EXPECT_EQ(result.line, 4);
        EXPECT_EQ(result.column, 1);
        EXPECT_EQ(result.source, "four");
    }
}

TEST(Lexer, skip_whitespace_eof) {
    {
        const auto [index, line, column] = skip_whitespace("    ", 1, 1);
        EXPECT_EQ(index, nullptr);
        EXPECT_EQ(line, 1);
        EXPECT_EQ(column, 5);
    }

    {
        const auto [index, line, column] = skip_whitespace("\n", 1, 1);
        EXPECT_EQ(index, nullptr);
        EXPECT_EQ(line, 2);
        EXPECT_EQ(column, 1);
    }

    {
        const auto [index, line, column] = skip_whitespace("// Comment with newline\n", 1, 1);
        EXPECT_EQ(index, nullptr);
        EXPECT_EQ(line, 2);
        EXPECT_EQ(column, 1);
    }

    {
        const auto [index, line, column] = skip_whitespace("// Comment without newline", 1, 1);
        EXPECT_EQ(index, nullptr);
        EXPECT_EQ(line, 1);
        EXPECT_EQ(column, 27);
    }
}

TEST(Lexer, find_keyword) {
    EXPECT_EQ(find_keyword("if"), TokenKind::If);
    EXPECT_EQ(find_keyword("else"), TokenKind::Else);
    EXPECT_EQ(find_keyword("true"), TokenKind::True);
    EXPECT_EQ(find_keyword("false"), TokenKind::False);

    EXPECT_EQ(find_keyword(std::string_view()), TokenKind::Undefined);
    EXPECT_EQ(find_keyword(""), TokenKind::Undefined);
    EXPECT_EQ(find_keyword(" "), TokenKind::Undefined);
}

TEST(Lexer, next) {
    {
        Lexer lexer("if else a b 1 2 + . , : ;");
        EXPECT_EQ(lexer.next().kind, TokenKind::If);
        EXPECT_EQ(lexer.next().kind, TokenKind::Else);
        EXPECT_EQ(lexer.next().kind, TokenKind::Identifier);
        EXPECT_EQ(lexer.next().kind, TokenKind::Identifier);
        EXPECT_EQ(lexer.next().kind, TokenKind::Integer);
        EXPECT_EQ(lexer.next().kind, TokenKind::Integer);
        EXPECT_EQ(lexer.next().kind, TokenKind::Plus);
        EXPECT_EQ(lexer.next().kind, TokenKind::Period);
        EXPECT_EQ(lexer.next().kind, TokenKind::Comma);
        EXPECT_EQ(lexer.next().kind, TokenKind::Colon);
        EXPECT_EQ(lexer.next().kind, TokenKind::Semicolon);
        EXPECT_EQ(lexer.next().kind, TokenKind::Eof);

        // Check that the end of file token is sticky, and is returned repeatedly
        EXPECT_EQ(lexer.next().kind, TokenKind::Eof);
    }
}

TEST(Lexer, single_char_operators) {
    {
        Lexer lexer("# . , : ;");
        EXPECT_EQ(lexer.next().kind, TokenKind::Hash);
        EXPECT_EQ(lexer.next().kind, TokenKind::Period);
        EXPECT_EQ(lexer.next().kind, TokenKind::Comma);
        EXPECT_EQ(lexer.next().kind, TokenKind::Colon);
        EXPECT_EQ(lexer.next().kind, TokenKind::Semicolon);
        EXPECT_EQ(lexer.next().kind, TokenKind::Eof);
    }

    {
        Lexer lexer("@ ! + - * / %");
        EXPECT_EQ(lexer.next().kind, TokenKind::At);
        EXPECT_EQ(lexer.next().kind, TokenKind::Exclaim);
        EXPECT_EQ(lexer.next().kind, TokenKind::Plus);
        EXPECT_EQ(lexer.next().kind, TokenKind::Minus);
        EXPECT_EQ(lexer.next().kind, TokenKind::Star);
        EXPECT_EQ(lexer.next().kind, TokenKind::Slash);
        EXPECT_EQ(lexer.next().kind, TokenKind::Percent);
        EXPECT_EQ(lexer.next().kind, TokenKind::Eof);
    }

    {
        Lexer lexer("( ) [ ] { }");
        EXPECT_EQ(lexer.next().kind, TokenKind::LRoundBracket);
        EXPECT_EQ(lexer.next().kind, TokenKind::RRoundBracket);
        EXPECT_EQ(lexer.next().kind, TokenKind::LSquareBracket);
        EXPECT_EQ(lexer.next().kind, TokenKind::RSquareBracket);
        EXPECT_EQ(lexer.next().kind, TokenKind::LCurlyBracket);
        EXPECT_EQ(lexer.next().kind, TokenKind::RCurlyBracket);
        EXPECT_EQ(lexer.next().kind, TokenKind::Eof);
    }
}

TEST(Lexer, multi_char_operators) {
    Lexer lexer("-> << >> == != <= >=");
    EXPECT_EQ(lexer.next().kind, TokenKind::RArrow);
    EXPECT_EQ(lexer.next().kind, TokenKind::LessLess);
    EXPECT_EQ(lexer.next().kind, TokenKind::GreaterGreater);
    EXPECT_EQ(lexer.next().kind, TokenKind::EqualEqual);
    EXPECT_EQ(lexer.next().kind, TokenKind::ExclaimEqual);
    EXPECT_EQ(lexer.next().kind, TokenKind::LessEqual);
    EXPECT_EQ(lexer.next().kind, TokenKind::GreaterEqual);
    EXPECT_EQ(lexer.next().kind, TokenKind::Eof);
}
