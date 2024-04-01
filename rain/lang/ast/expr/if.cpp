#include "rain/lang/ast/expr/if.hpp"

#include "absl/strings/str_cat.h"
#include "rain/lang/ast/scope/builtin.hpp"
#include "rain/lang/ast/var/variable.hpp"
#include "rain/lang/err/syntax.hpp"

namespace rain::lang::ast {

bool IfExpression::compile_time_capable() const noexcept {
    return _condition->compile_time_capable() && _then->compile_time_capable() &&
           (!_else.has_value() || _else.value()->compile_time_capable());
}

util::Result<void> IfExpression::validate(Options& options, Scope& scope) {
    {
        auto result = _condition->validate(options, scope);
        FORWARD_ERROR(result);
    }
    {
        auto result = _then->validate(options, scope);
        FORWARD_ERROR(result);
    }
    if (_else.has_value()) {
        auto result = _else.value()->validate(options, scope);
        FORWARD_ERROR(result);
    }

    if (_condition->type() != scope.builtin()->bool_type()) {
        return ERR_PTR(err::SyntaxError, _condition->location(),
                       "if condition must result in a boolean value");
    }

    if (_else.has_value()) {
        if (_then->type() != _else.value()->type()) {
            lex::Location location;
            if (_else.value()->expressions().empty()) {
                location = _else.value()->location();
            } else {
                location = _else.value()->expressions().back()->location();
            }

            return ERR_PTR(err::SyntaxError, location,
                           "if branches must (currently) have the same type");
        }
        _type = _then->type();
    } else {
        _type = nullptr;
    }

    return {};
}

}  // namespace rain::lang::ast
