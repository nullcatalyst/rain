#include "rain/lang/ast/expr/extern.hpp"

#include <array>
#include <span>
#include <tuple>

#include "rain/lang/ast/expr/extern_js_allowlist.hpp"
#include "rain/lang/ast/scope/builtin.hpp"
#include "rain/lang/err/syntax.hpp"

namespace rain::lang::ast {

bool ExternExpression::compile_time_capable() const noexcept {
    if (_keys.size() == 3 && _keys[0] == "js") {
        const auto found_namespace = std::lower_bound(
            JS_ALLOWED_NAMESPACES.begin(), JS_ALLOWED_NAMESPACES.end(), _keys[1],
            [](const auto& lhs, const auto& rhs) { return std::get<0>(lhs) < rhs; });

        if (found_namespace != JS_ALLOWED_NAMESPACES.end()) {
            const auto& [namespace_name, functions] = *found_namespace;
            const auto found_function =
                std::lower_bound(functions.begin(), functions.end(), _keys[2]);
            return found_function != functions.end();
        }
    }

    return false;
}

util::Result<void> ExternExpression::validate(Scope& scope) {
    if (_keys.size() == 0) {
        return ERR_PTR(err::SyntaxError, _location,
                       "extern expressions must have at least 1 key defining where the original "
                       "function is located");
    }

    if (_keys[0] != "js") {
        std::cout << "extern key0 " << _keys[0] << std::endl;
        return ERR_PTR(err::SyntaxError, _location,
                       "'js' is (currently) the only valid extern key");
    }

    if (_keys.size() != 3) {
        return ERR_PTR(err::SyntaxError, _location,
                       "extern 'js' functions must have 3 keys: 'js', the namespace, and the "
                       "function name");
    }

    return _declaration->validate(scope);
}

}  // namespace rain::lang::ast
