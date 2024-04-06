#include "rain/compile.hpp"

#include <string_view>

#include "rain/lang/ast/scope/builtin.hpp"
#include "rain/lang/code/context.hpp"
#include "rain/lang/code/expr/all.hpp"
#include "rain/lang/err/simple.hpp"
#include "rain/lang/lex/lazy.hpp"
#include "rain/lang/lex/lazy_list.hpp"
#include "rain/lang/parse/module.hpp"
#include "rain/lang/target/wasm/options.hpp"
#include "rain/util/result.hpp"

namespace rain {

using namespace lang;

util::Result<code::Module> compile(const std::string_view source) {
    wasm::Options options;
    return compile(source, options);
}

util::Result<code::Module> compile(const std::string_view source, Options& options) {
    auto               lexer      = lex::LazyLexer::using_source(source, "<unknown>");
    lex::LazyListLexer list_lexer = lex::LazyListLexer::using_lexer(lexer);

    ast::BuiltinScope builtin;
    auto              parse_result = parse::parse_module(lexer, builtin);
    FORWARD_ERROR(parse_result);
    auto parse_module = std::move(parse_result).value();

    auto validate_result = parse_module->validate(options);
    FORWARD_ERROR(validate_result);

    code::Module  code_module(options);
    code::Context ctx(code_module, options);
    code::compile_module(ctx, *parse_module);

    return code_module;
}

}  // namespace rain
