#include "rain/lang/ast/expr/extern.hpp"

#include <array>
#include <span>
#include <tuple>

#include "rain/lang/ast/scope/builtin.hpp"
#include "rain/lang/err/syntax.hpp"

namespace rain::lang::ast {

util::Result<void> ExternExpression::validate(Options& options, Scope& scope) {
    if (_keys.size() == 0) {
        return ERR_PTR(err::SyntaxError, _location,
                       "extern expressions must have at least 1 key defining where the original "
                       "function is located");
    }

    if (_keys[0] != "js") {
        return ERR_PTR(err::SyntaxError, _location,
                       "'js' is (currently) the only valid extern key");
    }

    if (_keys.size() != 3) {
        return ERR_PTR(err::SyntaxError, _location,
                       "extern 'js' functions must have 3 keys: 'js', the namespace, and the "
                       "function name");
    }

    _compile_time_capable = options.extern_is_compile_time_runnable(_keys);

    auto result = _declaration->validate(options, scope);
    FORWARD_ERROR(result);

    return {};
}

}  // namespace rain::lang::ast
