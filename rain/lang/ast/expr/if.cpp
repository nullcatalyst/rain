#include "rain/lang/ast/expr/if.hpp"

#include "absl/strings/str_cat.h"
#include "rain/lang/ast/scope/builtin.hpp"
#include "rain/lang/ast/var/variable.hpp"
#include "rain/lang/err/simple.hpp"

namespace rain::lang::ast {

bool IfExpression::compile_time_capable() const noexcept {
    return _condition->compile_time_capable() && _then->compile_time_capable() &&
           (!_else.has_value() || _else.value()->compile_time_capable());
}

util::Result<void> IfExpression::validate(Scope& scope) {
    {
        auto result = _condition->validate(scope);
        FORWARD_ERROR(result);
    }
    {
        auto result = _then->validate(scope);
        FORWARD_ERROR(result);
    }
    if (_else.has_value()) {
        auto result = _else.value()->validate(scope);
        FORWARD_ERROR(result);
    }

    if (_condition->type() != scope.builtin()->bool_type()) {
        return ERR_PTR(err::SimpleError, "if condition must result in a boolean value");
    }

    // TODO: Handle optional types
    assert(_else.has_value() && "optional types not yet implemented in if expressions");
    if (_then->type() != _else.value()->type()) {
        return ERR_PTR(err::SimpleError, "if branches must have the same type");
    }

    _type = _then->type();
    return {};
}

}  // namespace rain::lang::ast
