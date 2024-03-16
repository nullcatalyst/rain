#include <string_view>

#include "gtest/gtest.h"
#include "rain/lang/ast/scope/builtin.hpp"
#include "rain/lang/lex/lazy_lexer.hpp"
#include "rain/lang/lex/list_lexer.hpp"
#include "rain/lang/parse/all.hpp"

TEST(Lexer, lazy) {
    using namespace rain;
    using namespace rain::lang;

    const std::string_view code = R"(
fn main() {
    1
}
)";

    auto lexer = lex::LazyLexer::using_source(code);

    {
        const auto token = lexer.next();
        EXPECT_EQ(token.kind, lex::TokenKind::Fn);
        EXPECT_EQ(token.text(), "fn");
        EXPECT_EQ(token.location.line, 2);
        EXPECT_EQ(token.location.column, 1);
    }

    {
        const auto token = lexer.next();
        EXPECT_EQ(token.kind, lex::TokenKind::Identifier);
        EXPECT_EQ(token.text(), "main");
        EXPECT_EQ(token.location.line, 2);
        EXPECT_EQ(token.location.column, 4);
    }

    EXPECT_EQ(lexer.next().kind, lex::TokenKind::LRoundBracket);
    EXPECT_EQ(lexer.next().kind, lex::TokenKind::RRoundBracket);
    EXPECT_EQ(lexer.next().kind, lex::TokenKind::LCurlyBracket);
    EXPECT_EQ(lexer.next().kind, lex::TokenKind::Integer);
    EXPECT_EQ(lexer.next().kind, lex::TokenKind::RCurlyBracket);
    EXPECT_EQ(lexer.next().kind, lex::TokenKind::EndOfFile);
}
