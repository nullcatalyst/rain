#include "rain/lang/ast/expr/if.hpp"

#include "absl/strings/str_cat.h"
#include "rain/lang/ast/expr/identifier.hpp"
#include "rain/lang/ast/expr/unary_operator.hpp"
#include "rain/lang/ast/scope/builtin.hpp"
#include "rain/lang/ast/var/unwrapped_optional.hpp"
#include "rain/lang/ast/var/variable.hpp"
#include "rain/lang/err/if_type_mismatch.hpp"
#include "rain/lang/err/syntax.hpp"

namespace rain::lang::ast {

bool IfExpression::is_compile_time_capable() const noexcept {
    return _condition->is_compile_time_capable() && _then->is_compile_time_capable() &&
           (!_else.has_value() || _else.value()->is_compile_time_capable());
}

// bool IfExpression::is_constant() const noexcept {
//     return _condition->constant() && _then->constant() &&
//            (!_else.has_value() || _else.value()->constant());
// }

util::Result<void> IfExpression::validate(Options& options, Scope& scope) {
    {
        auto result = _condition->validate(options, scope);
        FORWARD_ERROR(result);
    }

    // Special case the scenario where the condition is ONLY an optional check.
    if (_condition->kind() == serial::ExpressionKind::UnaryOperator) {
        auto* unary = static_cast<UnaryOperatorExpression*>(_condition.get());
        if (unary->op() == serial::UnaryOperatorKind::HasValue &&
            unary->expression().kind() == serial::ExpressionKind::Variable &&
            unary->expression().type() != nullptr &&
            unary->expression().type()->kind() == serial::TypeKind::Optional &&
            unary->method()->is_builtin()) {
            auto& identifier = static_cast<IdentifierExpression&>(unary->expression());
            _then->scope().add_variable(
                std::make_unique<UnwrappedOptionalVariable>(identifier.variable()));
        }
    }

    {
        auto result = _then->validate(options, scope);
        FORWARD_ERROR(result);
    }
    if (_else.has_value()) {
        auto result = _else.value()->validate(options, scope);
        FORWARD_ERROR(result);
    }

    if (_condition->type() != scope.builtin()->bool_type()) {
        return ERR_PTR(err::SyntaxError, _condition->location(),
                       "if condition must result in a boolean value");
    }

    if (_else.has_value()) {
        if (_then->type() != _else.value()->type()) {
            lex::Location location;
            if (_else.value()->expressions().empty()) {
                location = _else.value()->location();
            } else {
                location = _else.value()->expressions().back()->location();
            }

            const auto get_error_info =
                [](const BlockExpression& block) -> std::tuple<lex::Location, std::string> {
                if (block.expressions().empty()) {
                    return {block.location(), "<null>"};
                } else {
                    return {block.expressions().back()->location(),
                            Type::display_name(block.expressions().back()->type())};
                }
            };

            auto [then_location, then_type] = get_error_info(*_then);
            auto [else_location, else_type] = get_error_info(*_else.value());
            return ERR_PTR(err::IfTypeMismatchError, then_location, std::move(then_type),
                           else_location, std::move(else_type));
        }

        _type = _then->type();
        return {};
    }

    if (_then->type() != nullptr) {
        _type = &_then->type()->get_optional_type(scope);
    } else {
        _type = nullptr;
    }
    return {};
}

}  // namespace rain::lang::ast
