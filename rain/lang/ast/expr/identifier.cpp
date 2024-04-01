#include "rain/lang/ast/expr/identifier.hpp"

#include "absl/strings/str_cat.h"
#include "rain/lang/ast/scope/builtin.hpp"
#include "rain/lang/ast/var/variable.hpp"
#include "rain/lang/err/syntax.hpp"

namespace rain::lang::ast {

util::Result<void> IdentifierExpression::validate(Options& options, Scope& scope) {
    _variable = scope.find_variable(_name);
    if (_variable == nullptr) {
        return ERR_PTR(err::SyntaxError, _location,
                       absl::StrCat("no variable named '", _name, "' found in scope"));
    }

    _type = _variable->type();
    return {};
}

}  // namespace rain::lang::ast
