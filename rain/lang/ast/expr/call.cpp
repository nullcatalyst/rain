#include "rain/lang/ast/expr/call.hpp"

#include "rain/lang/ast/expr/identifier.hpp"
#include "rain/lang/ast/expr/member.hpp"
#include "rain/lang/ast/var/function.hpp"
#include "rain/lang/err/simple.hpp"
#include "rain/util/log.hpp"

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

util::Result<void> CallExpression::validate(Scope& scope) {
    const auto validate_arguments =
        [&](absl::Nullable<Type*> callee_type) -> util::Result<Scope::TypeList> {
        Scope::TypeList argument_types;
        argument_types.reserve(_arguments.size() + 1);

        // Assumw that the function takes self as the first argument.
        if (callee_type != nullptr) {
            argument_types.push_back(callee_type);
        }

        for (auto& argument : _arguments) {
            auto result = argument->validate(scope);
            FORWARD_ERROR(result);

            argument_types.push_back(argument->type());
        }

        return argument_types;
    };

    // Specially handle the callee expression, in order to support function overloading.
    switch (_callee->kind()) {
        case serial::ExpressionKind::Member: {
            auto* member = reinterpret_cast<MemberExpression*>(_callee.get());
            auto  result = member->lhs()->validate(scope);
            FORWARD_ERROR(result);

            auto* callee_type           = member->lhs()->type();
            auto  argument_types_result = validate_arguments(callee_type);
            FORWARD_ERROR(argument_types_result);
            auto argument_types = std::move(argument_types_result).value();

            auto* function = scope.find_function(callee_type, argument_types, member->name());
            if (function == nullptr) {
                // Remove the self argument from the list.
                argument_types.erase(argument_types.begin());
                function = scope.find_function(callee_type, argument_types, member->name());

                if (function == nullptr) {
#if !defined(NDEBUG)
                    return ERR_PTR(err::SimpleError,
                                   absl::StrCat("no method named .", member->name(),
                                                " found on type ", callee_type->debug_name()));
#else
                    return ERR_PTR(
                        err::SimpleError,
                        absl::StrCat("no method named .", member->name(), " found on type"));
#endif  // !defined(NDEBUG)
                }
            }

            _function = function;
            _type     = function->function_type()->return_type();
            break;
        }

        case serial::ExpressionKind::Variable: {
            auto* identifier = reinterpret_cast<IdentifierExpression*>(_callee.get());

            auto argument_types_result = validate_arguments(nullptr);
            FORWARD_ERROR(argument_types_result);
            auto argument_types = std::move(argument_types_result).value();

            FunctionVariable* function =
                scope.find_function(nullptr, argument_types, identifier->name());
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
            auto result = _callee->validate(scope);
            FORWARD_ERROR(result);

            auto* callee_type = _callee->type();
            if (callee_type->kind() != serial::TypeKind::Function) {
                return ERR_PTR(err::SimpleError, "callee is not a function");
            }

            return ERR_PTR(err::SimpleError,
                           "calling a function expression is not yet implemented");
            break;
        }
    }

    return {};
}

}  // namespace rain::lang::ast
