#include "rain/lang/ast/expr/while.hpp"

#include "absl/strings/str_cat.h"
#include "rain/lang/ast/scope/builtin.hpp"
#include "rain/lang/ast/var/variable.hpp"
#include "rain/lang/err/syntax.hpp"

namespace rain::lang::ast {

bool WhileExpression::is_compile_time_capable() const noexcept {
    return _condition->is_compile_time_capable() && _loop->is_compile_time_capable() &&
           (!_else.has_value() || _else.value()->is_compile_time_capable());
}

util::Result<void> WhileExpression::validate(Options& options, Scope& scope) {
    {
        auto result = _condition->validate(options, scope);
        FORWARD_ERROR(result);
    }
    {
        auto result = _loop->validate(options, scope);
        FORWARD_ERROR(result);
    }
    if (_else.has_value()) {
        auto result = _else.value()->validate(options, scope);
        FORWARD_ERROR(result);
    }

    if (_condition->type() != scope.builtin()->bool_type()) {
        return ERR_PTR(err::SyntaxError, _condition->location(),
                       "while condition must result in a boolean value");
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
