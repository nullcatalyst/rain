
#include <string_view>

#include "gtest/gtest.h"
//
#include "rain/lang/code/target/default.hpp"
#include "rain/rain.hpp"

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

fn do_something() -> i32 {
    4.double()
}

fn fib(n: i32) -> i32 {
    if n < 2 {
        1
    } else {
        fib(n - 1) + fib(n - 2)
    }
}
)";

    rain::lang::code::initialize_llvm();

    auto module_result = rain::compile(code);
    ASSERT_TRUE(check_success(module_result));
    auto mod = std::move(module_result).value();

    mod.optimize();

    auto ir_result = mod.emit_ir();
    ASSERT_TRUE(check_success(ir_result));
    auto ir = std::move(ir_result).value();

    std::cout << ir << std::endl;

    // auto              lexer = lex::LazyLexer::using_source(code);
    // ast::BuiltinScope builtin_scope;
    // auto              module_result = parse::parse_module(lexer, builtin_scope);
    // ASSERT_TRUE(check_success(module_result));

    // auto parse_module = std::move(module_result).value();
    // ASSERT_EQ(parse_module->expressions().size(), 2);

    // auto validate_result = parse_module->validate();
    // ASSERT_TRUE(check_success(validate_result));

    // code::initialize_llvm();

    // code::Module  code_module;
    // code::Context ctx(code_module);
    // code::compile_module(ctx, parse_module);
    // // ASSERT_TRUE(check_success(compile_result));

    // code_module.optimize();
    // std::cout << code_module.emit_ir() << std::endl;

    // auto struct_point = mod->expressions()[0].get();
    // ASSERT_EQ(struct_point->kind(), ast::ExpressionKind::Struct);

    // auto main_function = mod->expressions()[1].get();
    // ASSERT_EQ(main_function->kind(), ast::ExpressionKind::Function);
}
