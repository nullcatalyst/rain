#include "rain/lang/ast/expr/call.hpp"

#include "rain/lang/ast/expr/identifier.hpp"
#include "rain/lang/ast/expr/member.hpp"
#include "rain/lang/ast/type/meta.hpp"
#include "rain/lang/ast/var/function.hpp"
#include "rain/lang/err/syntax.hpp"
#include "rain/util/console.hpp"

namespace rain::lang::ast {

bool CallExpression::compile_time_capable() const noexcept {
    if (_function == nullptr) {
        return (_callee == nullptr || _callee->compile_time_capable()) &&
               std::all_of(_arguments.begin(), _arguments.end(),
                           [](const auto& argument) { return argument->compile_time_capable(); });
    } else {
        return std::all_of(_arguments.begin(), _arguments.end(),
                           [](const auto& argument) { return argument->compile_time_capable(); });
    }
}

util::Result<void> CallExpression::validate(Options& options, Scope& scope) {
    Scope::TypeList argument_types;
    argument_types.reserve(_arguments.size() + 1);
    for (auto& argument : _arguments) {
        auto result = argument->validate(options, scope);
        FORWARD_ERROR(result);
        argument_types.emplace_back(argument->type());
    }

    // Specially handle the callee expression, in order to support function overloading.
    switch (_callee->kind()) {
        case serial::ExpressionKind::Member: {
            auto& member = static_cast<MemberExpression&>(*_callee.get());
            auto  result = member.lhs().validate(options, scope);
            FORWARD_ERROR(result);

            auto* callee_type = member.lhs().type();
            auto* function    = scope.find_method(member.name(), callee_type, argument_types);
            if (function == nullptr) {
                return ERR_PTR(err::SyntaxError, member.member_location(),
                               absl::StrCat("no method \"", member.name(), "\" found on type \"",
                                            callee_type->display_name(), "\""));
            }

            _function = function;
            _type     = function->function_type()->return_type();
            break;
        }

        case serial::ExpressionKind::Variable: {
            auto& identifier = static_cast<IdentifierExpression&>(*_callee.get());

            FunctionVariable* function =
                scope.find_function(identifier.name(), nullptr, argument_types);
            if (function != nullptr) {
                _function = function;
                _type     = function->function_type()->return_type();
                return {};
            }

            // The identifier is not referring to an overloaded function, so it must be a variable.
            // Fallthrough to have the variable be handled as a regular expression.
            [[fallthrough]];
        }

        default: {
            auto result = _callee->validate(options, scope);
            FORWARD_ERROR(result);

            auto* callee_type = _callee->type();
            if (callee_type->kind() != serial::TypeKind::Function) {
                return ERR_PTR(err::SyntaxError, _callee->location(), "callee is not a function");
            }

            return ERR_PTR(err::SyntaxError, _callee->location(),
                           "calling a function expression is not yet implemented");
            break;
        }
    }

    return {};
}

}  // namespace rain::lang::ast
