#include "rain/lang/parser.hpp"

#include <gtest/gtest.h>

#include "rain/ast/expr/all.hpp"
#include "rain/ast/type/all.hpp"

using namespace rain;
using namespace rain::lang;

TEST(Parser, parse_struct_good) {
    {
        // Empty struct
        Lexer      lexer("struct Foo { }");
        Parser     parser;
        const auto result = parser.parse_type(lexer);
        ASSERT_NOT_ERROR(result);

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
        ASSERT_NOT_ERROR(result);

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
        ASSERT_NOT_ERROR(result);

        const auto type = result.unwrap();
        ASSERT_EQ(ast::StructType::is(type), true);

        const auto struct_type = ast::StructType::from(type);
        EXPECT_EQ(struct_type.name(), "Foo");
        EXPECT_EQ(struct_type.fields().size(), 1);

        const auto& field = struct_type.fields()[0];
        EXPECT_EQ(field.name, "bar");
    }
}

TEST(Parser, parse_struct_bad) {
    {
        // Missing closing brace
        Lexer      lexer("struct Foo {");
        Parser     parser;
        const auto result = parser.parse_type(lexer);
        ASSERT_EQ(result.is_error(), true);
    }

    {
        // Missing field ending semicolon
        Lexer      lexer("struct Foo { x: I32 }");
        Parser     parser;
        const auto result = parser.parse_type(lexer);
        ASSERT_EQ(result.is_error(), true);
    }

    {
        // Missing field ending semicolon
        Lexer      lexer("struct Foo { x:; }");
        Parser     parser;
        const auto result = parser.parse_type(lexer);
        ASSERT_EQ(result.is_error(), true);
    }

    {
        // Missing field separating colon
        Lexer      lexer("struct Foo { x I32; }");
        Parser     parser;
        const auto result = parser.parse_type(lexer);
        ASSERT_EQ(result.is_error(), true);
    }

    {
        // Only field name
        Lexer      lexer("struct Foo { x }");
        Parser     parser;
        const auto result = parser.parse_type(lexer);
        ASSERT_EQ(result.is_error(), true);
    }
}

TEST(Parser, parse_integer_good) {
    Lexer      lexer("123");
    Parser     parser;
    const auto result = parser.parse_expression(lexer);
    ASSERT_NOT_ERROR(result);

    const auto expr = result.unwrap();
    ASSERT_EQ(ast::IntegerExpression::is(expr), true);

    const auto int_expr = ast::IntegerExpression::from(expr);
    EXPECT_EQ(int_expr.value(), 123);
}

TEST(Parser, parse_identifier_good) {
    Lexer      lexer("hello");
    Parser     parser;
    const auto result = parser.parse_expression(lexer);
    ASSERT_NOT_ERROR(result);

    const auto expr = result.unwrap();
    ASSERT_EQ(ast::IdentifierExpression::is(expr), true);

    const auto iden_expr = ast::IdentifierExpression::from(expr);
    EXPECT_EQ(iden_expr.name(), "hello");
}

TEST(Parser, parse_parenthesis_good) {
    Lexer      lexer("(hello)");
    Parser     parser;
    const auto result = parser.parse_expression(lexer);
    ASSERT_NOT_ERROR(result);

    const auto expr = result.unwrap();
    ASSERT_EQ(ast::ParenthesisExpression::is(expr), true);

    const auto paren_expr = ast::ParenthesisExpression::from(expr);
    // EXPECT_EQ(paren_expr.name(), "hello");
}

TEST(Parser, parse_member_good) {
    {
        // Single member
        Lexer      lexer("hello.world");
        Parser     parser;
        const auto result = parser.parse_expression(lexer);
        ASSERT_NOT_ERROR(result);

        const auto expr = result.unwrap();
        ASSERT_EQ(ast::MemberExpression::is(expr), true);
    }

    {
        // Nested member
        Lexer      lexer("hello.world.foo");
        Parser     parser;
        const auto result = parser.parse_expression(lexer);
        ASSERT_NOT_ERROR(result);

        const auto expr = result.unwrap();
        ASSERT_EQ(ast::MemberExpression::is(expr), true);
    }

    {
        // Member with arguments
        Lexer      lexer("hello(4).world");
        Parser     parser;
        const auto result = parser.parse_expression(lexer);
        ASSERT_NOT_ERROR(result);

        const auto expr = result.unwrap();
        ASSERT_EQ(ast::MemberExpression::is(expr), true);
    }
}

TEST(Parser, parse_unary_operator_good) {
    {
        // Compile-time executed expression
        Lexer      lexer("#fib(4)");
        Parser     parser;
        const auto result = parser.parse_expression(lexer);
        ASSERT_NOT_ERROR(result);

        const auto expr = result.unwrap();
        ASSERT_EQ(ast::ExecExpression::is(expr), true);
    }
}

TEST(Parser, parse_binary_operator_good) {
    Lexer      lexer("1 + 1");
    Parser     parser;
    const auto result = parser.parse_expression(lexer);
    ASSERT_NOT_ERROR(result);

    const auto expr = result.unwrap();
    ASSERT_EQ(ast::BinaryOperatorExpression::is(expr), true);

    const auto binop_expr = ast::BinaryOperatorExpression::from(expr);
    // EXPECT_EQ(paren_expr.name(), "hello");
}

TEST(Parser, parse_call_good) {
    {
        // No arguments
        Lexer      lexer("callMe()");
        Parser     parser;
        const auto result = parser.parse_expression(lexer);
        ASSERT_NOT_ERROR(result);

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
        ASSERT_NOT_ERROR(result);

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
        ASSERT_NOT_ERROR(result);

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
        ASSERT_NOT_ERROR(result);

        const auto expr = result.unwrap();
        ASSERT_EQ(ast::CallExpression::is(expr), true);

        const auto call_expr = ast::CallExpression::from(expr);
        EXPECT_EQ(call_expr.arguments().size(), 3);
    }

    {
        // Call a member function
        Lexer      lexer("hello.callMe(1, 2, 3)");
        Parser     parser;
        const auto result = parser.parse_expression(lexer);
        ASSERT_NOT_ERROR(result);

        const auto expr = result.unwrap();
        ASSERT_EQ(ast::CallExpression::is(expr), true);

        const auto call_expr = ast::CallExpression::from(expr);
        EXPECT_EQ(call_expr.arguments().size(), 3);
    }

    {
        // Mising function calls with binary operators
        Lexer      lexer("one() + two()");
        Parser     parser;
        const auto result = parser.parse_expression(lexer);
        ASSERT_NOT_ERROR(result);

        const auto expr = result.unwrap();
        ASSERT_EQ(ast::BinaryOperatorExpression::is(expr), true);

        const auto binop_expr = ast::BinaryOperatorExpression::from(expr);
        EXPECT_EQ(binop_expr.op(), ast::BinaryOperator::Add);

        const auto lhs = binop_expr.lhs();
        ASSERT_EQ(ast::CallExpression::is(lhs), true);

        const auto rhs = binop_expr.rhs();
        ASSERT_EQ(ast::CallExpression::is(rhs), true);
    }
}

TEST(Parser, parse_call_bad) {
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

    {
        // Missing separating comma
        Lexer      lexer("callMe(1 2)");
        Parser     parser;
        const auto result = parser.parse_expression(lexer);
        ASSERT_EQ(result.is_error(), true);
    }

    {
        // Duplicated comma
        Lexer      lexer("callMe(1,, 2)");
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
        ASSERT_NOT_ERROR(result);

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
        ASSERT_NOT_ERROR(result);

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
        ASSERT_NOT_ERROR(result);

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

TEST(Parser, parse_if_good) {
    {
        // Without else
        Lexer      lexer("if 0 { 1 }");
        Parser     parser;
        const auto result = parser.parse_expression(lexer);
        ASSERT_NOT_ERROR(result);

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
        ASSERT_NOT_ERROR(result);

        const auto expr = result.unwrap();
        ASSERT_EQ(ast::IfExpression::is(expr), true);

        const auto if_expr = ast::IfExpression::from(expr);
        // EXPECT_EQ(if_expr.expressions().size(), 1);
    }
}

TEST(Parser, parse_function_good) {
    {
        // No name, no arguments, no return type, no body
        Lexer      lexer("fn() {}");
        Parser     parser;
        const auto result = parser.parse_expression(lexer);
        ASSERT_NOT_ERROR(result);

        const auto expr = result.unwrap();
        ASSERT_EQ(ast::FunctionExpression::is(expr), true);

        const auto fn_expr = ast::FunctionExpression::from(expr);
        EXPECT_EQ(fn_expr.expressions().size(), 0);
    }

    {
        // No name, no arguments, no return type, with body
        Lexer      lexer("fn() { 1 }");
        Parser     parser;
        const auto result = parser.parse_expression(lexer);
        ASSERT_NOT_ERROR(result);

        const auto expr = result.unwrap();
        ASSERT_EQ(ast::FunctionExpression::is(expr), true);

        const auto fn_expr = ast::FunctionExpression::from(expr);
        EXPECT_EQ(fn_expr.expressions().size(), 1);
    }

    {
        // No name, no arguments, with return type, with body
        Lexer      lexer("fn() -> i32 { 1 }");
        Parser     parser;
        const auto result = parser.parse_expression(lexer);
        ASSERT_NOT_ERROR(result);

        const auto expr = result.unwrap();
        ASSERT_EQ(ast::FunctionExpression::is(expr), true);

        const auto fn_expr = ast::FunctionExpression::from(expr);
        EXPECT_EQ(fn_expr.expressions().size(), 1);
    }

    {
        // No name, with arguments, with return type, with body
        Lexer      lexer("fn(x: i32, y: i32) -> i32 { 1 }");
        Parser     parser;
        const auto result = parser.parse_expression(lexer);
        ASSERT_NOT_ERROR(result);

        const auto expr = result.unwrap();
        ASSERT_EQ(ast::FunctionExpression::is(expr), true);

        const auto fn_expr = ast::FunctionExpression::from(expr);
        EXPECT_EQ(fn_expr.expressions().size(), 1);
    }

    {
        // With name, with arguments, with return type, with body
        Lexer      lexer("fn foo(x: i32, y: i32) -> i32 { 1 }");
        Parser     parser;
        const auto result = parser.parse_expression(lexer);
        ASSERT_NOT_ERROR(result);

        const auto expr = result.unwrap();
        ASSERT_EQ(ast::FunctionExpression::is(expr), true);

        const auto fn_expr = ast::FunctionExpression::from(expr);
        EXPECT_EQ(fn_expr.expressions().size(), 1);
    }
}

TEST(Parser, parse_export_good) {
    {
        // Export a single function
        Lexer      lexer("export fn one() { return 1 }");
        Parser     parser;
        const auto result = parser.parse(lexer);
        ASSERT_NOT_ERROR(result);

        ASSERT_EQ(result.unwrap().nodes().size(), 1);
    }

    // {
    //     // Export two functions
    //     Lexer      lexer("export fn one() { 1 }\nexport fn two() { 2 }\n");
    //     Parser     parser;
    //     const auto result = parser.parse(lexer);
    //     ASSERT_NOT_ERROR(result);

    //     ASSERT_EQ(result.unwrap().nodes().size(), 2);
    // }
}
