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
    const auto validate_arguments =
        [&](absl::Nullable<Type*> callee_type) -> util::Result<Scope::TypeList> {
        Scope::TypeList argument_types;
        argument_types.reserve(_arguments.size() + 1);

        // Assumw that the function takes self as the first argument.
        if (callee_type != nullptr) {
            argument_types.push_back(callee_type);
        }

        for (auto& argument : _arguments) {
            auto result = argument->validate(options, scope);
            FORWARD_ERROR(result);

            argument_types.push_back(argument->type());
        }

        return argument_types;
    };

    // Specially handle the callee expression, in order to support function overloading.
    switch (_callee->kind()) {
        case serial::ExpressionKind::Member: {
            auto& member = *reinterpret_cast<MemberExpression*>(_callee.get());
            auto  result = member.lhs().validate(options, scope);
            FORWARD_ERROR(result);

            auto* callee_type = member.lhs().type();
            bool  try_self    = true;

            // If the variable is a meta variable (a type), then we can try to search for a method
            // on that type that doesn't take self as a parameter.
            if (callee_type->kind() == serial::TypeKind::Meta) {
                auto* meta_type = reinterpret_cast<MetaType*>(callee_type);
                callee_type     = &meta_type->type();
                try_self        = false;
            }

            auto argument_types_result = validate_arguments(try_self ? callee_type : nullptr);
            FORWARD_ERROR(argument_types_result);
            auto argument_types = std::move(argument_types_result).value();

            auto* function = scope.find_function(callee_type, argument_types, member.name());
            if (function == nullptr) {
                if (try_self) {
                    // Remove the self argument from the list.
                    argument_types.erase(argument_types.begin());
                    function = scope.find_function(callee_type, argument_types, member.name());
                }

                if (function == nullptr) {
                    return ERR_PTR(err::SyntaxError, member.member_location(),
                                   absl::StrCat("no method .", member.name(), " found on type ",
                                                callee_type->name()));
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
