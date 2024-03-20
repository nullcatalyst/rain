#include "rain/lang/ast/expr/call.hpp"

#include "rain/lang/ast/expr/member.hpp"
#include "rain/lang/ast/var/function.hpp"
#include "rain/lang/err/simple.hpp"
#include "rain/util/log.hpp"

namespace rain::lang::ast {

util::Result<void> CallExpression::validate(Scope& scope) {
    // Specially handle the callee expression, in order to support function overloading.
    absl::Nonnull<Type*> callee_type;
    switch (_callee->kind()) {
        case serial::ExpressionKind::Variable: {
            util::console_error("calling a variable expression is not yet implemented");
            std::abort();
            break;
        }

        case serial::ExpressionKind::Member: {
            auto* member = reinterpret_cast<MemberExpression*>(_callee.get());
            auto  result = member->lhs()->validate(scope);
            FORWARD_ERROR(result);

            callee_type = member->lhs()->type();
            break;
        }

        default: {
            auto result = _callee->validate(scope);
            FORWARD_ERROR(result);

            callee_type = _callee->type();
            if (callee_type->kind() != serial::TypeKind::Function) {
                return ERR_PTR(err::SimpleError, "callee is not a function");
            }
            break;
        }
    }

    Scope::TypeList argument_types;
    argument_types.reserve(_arguments.size() + 1);

    // Start by assuming that the method takes self as the first argument.
    argument_types.push_back(callee_type);
    for (auto& argument : _arguments) {
        auto result = argument->validate(scope);
        FORWARD_ERROR(result);

        argument_types.push_back(argument->type());
    }

    if (_callee->kind() == serial::ExpressionKind::Member) {
        auto* member = reinterpret_cast<MemberExpression*>(_callee.get());
        auto* method = scope.find_method(member->lhs()->type(), argument_types, member->name());

        if (method == nullptr) {
            // Remove the self argument from the list.
            argument_types.erase(argument_types.begin());
            method = scope.find_method(member->lhs()->type(), argument_types, member->name());

            if (method == nullptr) {
#if !defined(NDEBUG)
                return ERR_PTR(err::SimpleError,
                               absl::StrCat("no method named .", member->name(), " found on type ",
                                            member->lhs()->type()->debug_name()));
#else
                return ERR_PTR(err::SimpleError,
                               absl::StrCat("no method named .", member->name(), " found on type"));
#endif  // !defined(NDEBUG)
            }
        }

        _function = method;
        _type     = method->function_type()->return_type();
    } else {
        return ERR_PTR(err::SimpleError, "callee is not a member expression");
    }

    return {};
}

}  // namespace rain::lang::ast
