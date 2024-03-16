#include <string_view>

#include "gtest/gtest.h"
#include "rain/lang/ast/scope/builtin.hpp"
#include "rain/lang/lex/lazy_lexer.hpp"
#include "rain/lang/lex/list_lexer.hpp"
#include "rain/lang/parse/all.hpp"

TEST(Lang, struct) {
    //     const std::string_view code = R"(
    // struct Point {
    //     x: i32,
    //     y: i32,
    // }

    // fn Point.__add__(self, other: Point) -> Point {
    //     Point{
    //         x: self.x + other.x,
    //         y: self.y + other.y,
    //     }
    // }
    // )";
    const std::string_view code = R"(
fn i32.double(self) -> i32 {
    2 * self
}
)";

    using namespace rain;
    using namespace rain::lang;

    auto              lexer = lex::LazyLexer::using_source(code);
    ast::BuiltinScope builtin_scope;
    auto              module_result = parse::parse_module(lexer, builtin_scope);
    ASSERT_TRUE(check_success(module_result));

    auto mod = std::move(module_result).value();
    ASSERT_EQ(mod->expressions().size(), 2);

    // auto struct_point = mod->expressions()[0].get();
    // ASSERT_EQ(struct_point->kind(), ast::ExpressionKind::Struct);

    // auto main_function = mod->expressions()[1].get();
    // ASSERT_EQ(main_function->kind(), ast::ExpressionKind::Function);
}
