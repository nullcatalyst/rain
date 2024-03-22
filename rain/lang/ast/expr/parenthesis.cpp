#include "rain/lang/ast/expr/parenthesis.hpp"

#include <optional>
#include <string_view>

#include "absl/strings/str_cat.h"
#include "rain/lang/err/simple.hpp"

namespace rain::lang::ast {

util::Result<void> ParenthesisExpression::validate(Scope& scope) {
    return _expression->validate(scope);
}

}  // namespace rain::lang::ast
