#include "rain/lang/ast/type/unresolved.hpp"

#include "absl/strings/str_cat.h"
#include "rain/lang/ast/expr/expression.hpp"
#include "rain/lang/ast/scope/scope.hpp"
#include "rain/lang/err/syntax.hpp"
#include "rain/util/console.hpp"

namespace rain::lang::ast {

util::Result<absl::Nonnull<Type*>> UnresolvedType::_resolve(Options& options, Scope& scope) {
    auto resolved_type = scope.find_named_type(_name);
    if (resolved_type == nullptr) {
        return ERR_PTR(err::SyntaxError, _location,
                       absl::StrCat("no type \"", _name, "\" found in scope"));
    }

    return resolved_type;
}

}  // namespace rain::lang::ast
