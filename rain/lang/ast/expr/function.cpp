#include "rain/lang/ast/expr/function.hpp"

#include "rain/lang/ast/var/function.hpp"

namespace rain::lang::ast {

FunctionExpression::FunctionExpression(absl::Nullable<FunctionVariable*> variable,
                                       ArgumentList                      arguments,
                                       absl::Nonnull<FunctionType*>      function_type,
                                       std::unique_ptr<BlockExpression>  block,
                                       lex::Location                     declaration_location)
    : FunctionDeclarationExpression(variable, std::move(arguments), function_type,
                                    declaration_location),
      _block(std::move(block)) {}

util::Result<void> FunctionExpression::validate(Options& options, Scope& scope) {
    {
        auto result = _validate_declaration(options, scope);
        FORWARD_ERROR(result);
    }

    return _block->validate(options, scope);
}

}  // namespace rain::lang::ast
