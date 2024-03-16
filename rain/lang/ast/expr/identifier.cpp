#include "rain/lang/ast/expr/identifier.hpp"

#include "rain/lang/ast/scope/builtin.hpp"

namespace rain::lang::ast {

util::Result<void> IdentifierExpression::validate(Scope& scope) {
    _variable = scope.find_variable(_name);
    if (_variable == nullptr) {
        return ERR(err::UndefinedVariable, _location,
                   absl::StrCat("no variable named '", _name, "' found in scope"));
    }

    _type = _variable->type();
    return {};
}

}  // namespace rain::lang::ast
