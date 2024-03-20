#include <string_view>

#include "rain/compile.hpp"
#include "rain/lang/ast/scope/builtin.hpp"
#include "rain/lang/code/compile/all.hpp"
#include "rain/lang/code/context.hpp"
#include "rain/lang/err/simple.hpp"
#include "rain/lang/lex/lazy.hpp"
// #include "rain/lang/lex/lazy_list.hpp"
#include "rain/lang/parse/all.hpp"
#include "rain/util/result.hpp"

namespace rain {

using namespace lang;

util::Result<code::Module> compile(const std::string_view source) {
    return compile(source, []() {});
}

util::Result<code::Module> compile(const std::string_view     source,
                                   llvm::function_ref<void()> init_compiler) {
    auto lexer = lex::LazyLexer::using_source(source, "<unknown>");
    // lex::LazyListLexer list_lexer = lex::LazyListLexer::using_lexer(lexer);

    ast::BuiltinScope builtin;
    auto              parse_result = parse::parse_module(lexer, builtin);
    FORWARD_ERROR(parse_result);
    auto parse_module = std::move(parse_result).value();

    auto validate_result = parse_module->validate();
    FORWARD_ERROR(validate_result);

    code::Module code_module;

    code::Context ctx(code_module);
    code::compile_module(ctx, *parse_module);

    return code_module;
}

}  // namespace rain
