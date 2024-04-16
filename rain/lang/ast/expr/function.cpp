#include "rain/lang/ast/expr/function.hpp"

#include "rain/lang/ast/var/function.hpp"
#include "rain/lang/err/return_type_mismatch.hpp"

namespace rain::lang::ast {

FunctionExpression::FunctionExpression(absl::Nullable<FunctionVariable*> variable,
                                       ArgumentList                      arguments,
                                       absl::Nonnull<FunctionType*>      function_type,
                                       std::unique_ptr<BlockExpression>  block,
                                       lex::Location                     declaration_location,
                                       lex::Location                     return_type_location)
    : FunctionDeclarationExpression(variable, std::move(arguments), function_type,
                                    declaration_location, return_type_location),
      _block(std::move(block)) {}

util::Result<void> FunctionExpression::validate(Options& options, Scope& scope) {
    {
        auto result = _validate_declaration(options, scope);
        FORWARD_ERROR(result);
    }

    {
        auto result = _block->validate(options, scope);
        FORWARD_ERROR(result);
    }

    if (function_type()->return_type() == nullptr) {
        // There is no return value expected, no need to check the block's return type.
        return {};
    }

    if (_block->type() == nullptr) {
        if (function_type()->return_type()->kind() == serial::TypeKind::Optional) {
            // It does not matter that the block is not returning a value, because the function
            // returns an optional type. This means the function is just always returning `none`.
            return {};
        }

        // The block is not returning a value, but the function does not return an optional. This
        // means that a value should have been returned, but none was.
        return ERR_PTR(err::ReturnTypeMismatchError,
                       _block->expressions().empty() ? _block->location()
                                                     : _block->expressions().back()->location(),
                       _block->type() == nullptr ? "<null>" : _block->type()->display_name(),
                       _return_type_location, function_type()->return_type()->display_name());
    }

    if (&Type::unwrap(*_block->type()) != &Type::unwrap(*function_type()->return_type())) {
        return ERR_PTR(err::ReturnTypeMismatchError,
                       _block->expressions().empty() ? _block->location()
                                                     : _block->expressions().back()->location(),
                       _block->type()->display_name(), _return_type_location,
                       function_type()->return_type()->display_name());
    }

    return {};
}

}  // namespace rain::lang::ast
