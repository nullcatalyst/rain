#include "cirrus/lang/parser.hpp"

#include <gtest/gtest.h>

#include "cirrus/ast/expr/all.hpp"
#include "cirrus/ast/type/all.hpp"

using namespace cirrus;
using namespace cirrus::lang;

TEST(Parser, parse_struct) {
    {
        // Empty struct
        Lexer      lexer("struct Foo { }");
        Parser     parser;
        const auto result = parser.parse_type(lexer);
        if (result.is_error()) {
            std::cerr << result.unwrap_error()->message() << std::endl;
            ASSERT_EQ(result.is_ok(), true);
        }

        const auto type = result.unwrap();
        ASSERT_EQ(ast::StructType::is(type), true);

        const auto struct_type = ast::StructType::from(type);
        EXPECT_EQ(struct_type.name(), "Foo");
        EXPECT_EQ(struct_type.fields().size(), 0);
    }

    {
        // Unnamed struct
        Lexer      lexer("struct { }");
        Parser     parser;
        const auto result = parser.parse_type(lexer);
        if (result.is_error()) {
            std::cerr << result.unwrap_error()->message() << std::endl;
            ASSERT_EQ(result.is_ok(), true);
        }

        const auto type = result.unwrap();
        ASSERT_EQ(ast::StructType::is(result.unwrap()), true);

        const auto struct_type = ast::StructType::from(type);
        EXPECT_EQ(struct_type.is_named(), false);
        EXPECT_EQ(struct_type.fields().size(), 0);
    }

    {
        // Struct with one field
        Lexer      lexer("struct Foo {\n    bar: i32;\n}\n");
        Parser     parser;
        const auto result = parser.parse_type(lexer);
        if (result.is_error()) {
            std::cerr << result.unwrap_error()->message() << std::endl;
            ASSERT_EQ(result.is_ok(), true);
        }

        const auto type = result.unwrap();
        ASSERT_EQ(ast::StructType::is(type), true);

        const auto struct_type = ast::StructType::from(type);
        EXPECT_EQ(struct_type.name(), "Foo");
        EXPECT_EQ(struct_type.fields().size(), 1);

        const auto& field = struct_type.fields()[0];
        EXPECT_EQ(field.name, "bar");
    }
}

TEST(Parser, parse_integer) {
    Lexer      lexer("123");
    Parser     parser;
    const auto result = parser.parse_expression(lexer);
    if (result.is_error()) {
        std::cerr << result.unwrap_error()->message() << std::endl;
        ASSERT_EQ(result.is_ok(), true);
    }

    const auto expr = result.unwrap();
    ASSERT_EQ(ast::IntegerExpression::is(expr), true);

    const auto int_expr = ast::IntegerExpression::from(expr);
    EXPECT_EQ(int_expr.value(), 123);
}

TEST(Parser, parse_identifier) {
    Lexer      lexer("hello");
    Parser     parser;
    const auto result = parser.parse_expression(lexer);
    if (result.is_error()) {
        std::cerr << result.unwrap_error()->message() << std::endl;
        ASSERT_EQ(result.is_ok(), true);
    }

    const auto expr = result.unwrap();
    ASSERT_EQ(ast::IdentifierExpression::is(expr), true);

    const auto iden_expr = ast::IdentifierExpression::from(expr);
    EXPECT_EQ(iden_expr.name(), "hello");
}

TEST(Parser, parse_parenthesis) {
    Lexer      lexer("(hello)");
    Parser     parser;
    const auto result = parser.parse_expression(lexer);
    if (result.is_error()) {
        std::cerr << result.unwrap_error()->message() << std::endl;
        ASSERT_EQ(result.is_ok(), true);
    }

    const auto expr = result.unwrap();
    ASSERT_EQ(ast::ParenthesisExpression::is(expr), true);

    const auto paren_expr = ast::ParenthesisExpression::from(expr);
    // EXPECT_EQ(paren_expr.name(), "hello");
}

TEST(Parser, parse_binary_operator) {
    Lexer      lexer("1 + 1");
    Parser     parser;
    const auto result = parser.parse_expression(lexer);
    if (result.is_error()) {
        std::cerr << result.unwrap_error()->message() << std::endl;
        ASSERT_EQ(result.is_ok(), true);
    }

    const auto expr = result.unwrap();
    ASSERT_EQ(ast::BinaryOperatorExpression::is(expr), true);

    const auto binop_expr = ast::BinaryOperatorExpression::from(expr);
    // EXPECT_EQ(paren_expr.name(), "hello");
}

TEST(Parser, parse_call) {
    {
        // No arguments
        Lexer      lexer("callMe()");
        Parser     parser;
        const auto result = parser.parse_expression(lexer);
        if (result.is_error()) {
            std::cerr << result.unwrap_error()->message() << std::endl;
            ASSERT_EQ(result.is_ok(), true);
        }

        const auto expr = result.unwrap();
        ASSERT_EQ(ast::CallExpression::is(expr), true);

        const auto call_expr = ast::CallExpression::from(expr);
        EXPECT_EQ(call_expr.arguments().size(), 0);
    }

    {
        // One argument
        Lexer      lexer("callMe(1)");
        Parser     parser;
        const auto result = parser.parse_expression(lexer);
        if (result.is_error()) {
            std::cerr << result.unwrap_error()->message() << std::endl;
            ASSERT_EQ(result.is_ok(), true);
        }

        const auto expr = result.unwrap();
        ASSERT_EQ(ast::CallExpression::is(expr), true);

        const auto call_expr = ast::CallExpression::from(expr);
        EXPECT_EQ(call_expr.arguments().size(), 1);
    }

    {
        // Multiple arguments, no trailing comma
        Lexer      lexer("callMe(1, 2, 3)");
        Parser     parser;
        const auto result = parser.parse_expression(lexer);
        if (result.is_error()) {
            std::cerr << result.unwrap_error()->message() << std::endl;
            ASSERT_EQ(result.is_ok(), true);
        }

        const auto expr = result.unwrap();
        ASSERT_EQ(ast::CallExpression::is(expr), true);

        const auto call_expr = ast::CallExpression::from(expr);
        EXPECT_EQ(call_expr.arguments().size(), 3);
    }

    {
        // Multiple arguments, trailing comma
        Lexer      lexer("callMe(1, 2, 3, )");
        Parser     parser;
        const auto result = parser.parse_expression(lexer);
        if (result.is_error()) {
            std::cerr << result.unwrap_error()->message() << std::endl;
            ASSERT_EQ(result.is_ok(), true);
        }

        const auto expr = result.unwrap();
        ASSERT_EQ(ast::CallExpression::is(expr), true);

        const auto call_expr = ast::CallExpression::from(expr);
        EXPECT_EQ(call_expr.arguments().size(), 3);
    }

    {
        // Missing closing parenthesis
        Lexer      lexer("callMe(1, 2, 3");
        Parser     parser;
        const auto result = parser.parse_expression(lexer);
        ASSERT_EQ(result.is_error(), true);
    }

    {
        // Leading comma
        Lexer      lexer("callMe(, 1, 2, 3)");
        Parser     parser;
        const auto result = parser.parse_expression(lexer);
        ASSERT_EQ(result.is_error(), true);
    }
}

TEST(Parser, parse_block) {
    {
        // Empty block
        Lexer      lexer("{ }");
        Parser     parser;
        const auto result = parser.parse_expression(lexer);
        if (result.is_error()) {
            std::cerr << result.unwrap_error()->message() << std::endl;
            ASSERT_EQ(result.is_ok(), true);
        }

        const auto expr = result.unwrap();
        ASSERT_EQ(ast::BlockExpression::is(expr), true);

        const auto block_expr = ast::BlockExpression::from(expr);
        EXPECT_EQ(block_expr.expressions().size(), 0);
    }

    {
        // One expression
        Lexer      lexer("{ 1 }");
        Parser     parser;
        const auto result = parser.parse_expression(lexer);
        if (result.is_error()) {
            std::cerr << result.unwrap_error()->message() << std::endl;
            ASSERT_EQ(result.is_ok(), true);
        }

        const auto expr = result.unwrap();
        ASSERT_EQ(ast::BlockExpression::is(expr), true);

        const auto block_expr = ast::BlockExpression::from(expr);
        EXPECT_EQ(block_expr.expressions().size(), 1);
    }

    {
        // Three expressions
        Lexer      lexer("{ 1 2 3 }");
        Parser     parser;
        const auto result = parser.parse_expression(lexer);
        if (result.is_error()) {
            std::cerr << result.unwrap_error()->message() << std::endl;
            ASSERT_EQ(result.is_ok(), true);
        }

        const auto expr = result.unwrap();
        ASSERT_EQ(ast::BlockExpression::is(expr), true);

        const auto block_expr = ast::BlockExpression::from(expr);
        EXPECT_EQ(block_expr.expressions().size(), 3);
    }

    {
        // Unbalanced braces
        Lexer      lexer("{ 1 2 3 ");
        Parser     parser;
        const auto result = parser.parse_expression(lexer);
        ASSERT_EQ(result.is_error(), true);
    }
}

TEST(Parser, parse_if) {
    {
        // Without else
        Lexer      lexer("if 0 { 1 }");
        Parser     parser;
        const auto result = parser.parse_expression(lexer);
        if (result.is_error()) {
            std::cerr << result.unwrap_error()->message() << std::endl;
            ASSERT_EQ(result.is_ok(), true);
        }

        const auto expr = result.unwrap();
        ASSERT_EQ(ast::IfExpression::is(expr), true);

        const auto if_expr = ast::IfExpression::from(expr);
        // EXPECT_EQ(if_expr.expressions().size(), 1);
    }

    {
        // With else
        Lexer      lexer("if 0 { 1 } else { 2 }");
        Parser     parser;
        const auto result = parser.parse_expression(lexer);
        if (result.is_error()) {
            std::cerr << result.unwrap_error()->message() << std::endl;
            ASSERT_EQ(result.is_ok(), true);
        }

        const auto expr = result.unwrap();
        ASSERT_EQ(ast::IfExpression::is(expr), true);

        const auto if_expr = ast::IfExpression::from(expr);
        // EXPECT_EQ(if_expr.expressions().size(), 1);
    }
}
