#include "rain/lang/ast/expr/while.hpp"

#include "absl/strings/str_cat.h"
#include "rain/lang/ast/scope/builtin.hpp"
#include "rain/lang/ast/var/variable.hpp"
#include "rain/lang/err/simple.hpp"

namespace rain::lang::ast {

bool WhileExpression::compile_time_capable() const noexcept {
    return _condition->compile_time_capable() && _loop->compile_time_capable() &&
           (!_else.has_value() || _else.value()->compile_time_capable());
}

util::Result<void> WhileExpression::validate(Scope& scope) {
    {
        auto result = _condition->validate(scope);
        FORWARD_ERROR(result);
    }
    {
        auto result = _loop->validate(scope);
        FORWARD_ERROR(result);
    }
    if (_else.has_value()) {
        auto result = _else.value()->validate(scope);
        FORWARD_ERROR(result);
    }

    if (_condition->type() != scope.builtin()->bool_type()) {
        return ERR_PTR(err::SimpleError, "while condition must result in a boolean value");
    }

    // TODO: Handle optional types
    assert(!_else.has_value() && "optional types not yet implemented in while expressions");
    // if (_loop->type() != _else.value()->type()) {
    //     return ERR_PTR(err::SimpleError, "while branches must have the same type");
    // }

    // _type = _then->type();
    _type = nullptr;
    return {};
}

}  // namespace rain::lang::ast
