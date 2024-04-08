#include "rain/lang/ast/expr/type.hpp"

#include "rain/lang/ast/type/struct.hpp"

namespace rain::lang::ast {

util::Result<void> TypeExpression::validate(Options& options, Scope& scope) {
    {
        auto result = _declare_type->resolve(options, scope);
        FORWARD_ERROR(result);

        _declare_type = std::move(result).value();
    }

    return {};
}

}  // namespace rain::lang::ast
