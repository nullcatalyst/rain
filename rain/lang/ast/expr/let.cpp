#include "rain/lang/ast/expr/let.hpp"

#include "absl/strings/str_cat.h"
#include "rain/lang/ast/scope/builtin.hpp"
#include "rain/lang/ast/var/block.hpp"
#include "rain/lang/ast/var/variable.hpp"
#include "rain/lang/err/simple.hpp"

namespace rain::lang::ast {

util::Result<void> LetExpression::validate(Scope& scope) {
    {
        auto result = _value->validate(scope);
        FORWARD_ERROR(result);
    }

    if (type() == nullptr) {
        return ERR_PTR(err::SimpleError, "let variable declaration initial value must have a type");
    }

    _variable = scope.add_variable(_name, std::make_unique<BlockVariable>(_name, type(), true));
    return {};
}

}  // namespace rain::lang::ast
