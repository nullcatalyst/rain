#include "rain/lang/ast/expr/function.hpp"

#include "rain/lang/ast/var/function.hpp"

namespace rain::lang::ast {

util::Result<void> FunctionExpression::validate(Options& options, Scope& scope) {
    {
        auto result = _validate_declaration(options, scope);
        FORWARD_ERROR(result);
    }

    _variable = scope.add_function(nullptr, _type->argument_types(), _name,
                                   std::make_unique<FunctionVariable>(_name, _type));

    return _block->validate(options, scope);
}

}  // namespace rain::lang::ast
