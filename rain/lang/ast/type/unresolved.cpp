#include "rain/lang/ast/type/unresolved.hpp"

#include "absl/strings/str_cat.h"
#include "rain/lang/ast/scope/scope.hpp"
#include "rain/lang/err/simple.hpp"

namespace rain::lang::ast {

util::Result<absl::Nonnull<Type*>> UnresolvedType::resolve(Options& options, Scope& scope) {
    auto resolved_type = scope.find_type(_name);
    if (resolved_type == nullptr) {
        // TODO: Add a location to the error
        return ERR_PTR(err::SimpleError, absl::StrCat("no type \"", _name, "\" found in scope"));
    }

    return resolved_type;
}

}  // namespace rain::lang::ast
