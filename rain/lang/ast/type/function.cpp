#include "rain/lang/ast/type/function.hpp"

#include "rain/lang/ast/scope/scope.hpp"

namespace rain::lang::ast {

FunctionType::FunctionType(absl::Nullable<Type*> callee_type, ArgumentTypeList argument_types,
                           absl::Nullable<Type*> return_type, bool self_argument)
    : _callee_type(callee_type),
      _argument_types(argument_types),
      _return_type(return_type),
      _self_argument(self_argument) {}

std::string FunctionType::display_name() const noexcept {
    std::string argument_type_names;
    for (size_t i = 0; i < _argument_types.size(); ++i) {
        if (i > 0) {
            absl::StrAppend(&argument_type_names, ", ", argument_type_names,
                            _argument_types[i]->display_name());
        } else {
            argument_type_names = _argument_types[i]->display_name();
        }
    }

    std::string callee_prefix;
    if (callee_type() != nullptr) {
        callee_prefix = absl::StrCat(callee_type()->display_name(), ".");
    }

    if (_return_type != nullptr) {
        return absl::StrCat(callee_prefix, "fn(", argument_type_names, ") -> ",
                            _return_type->display_name());
    } else {
        return absl::StrCat(callee_prefix, "fn(", argument_type_names, ")");
    }
}

util::Result<absl::Nonnull<Type*>> FunctionType::resolve(Options& options, Scope& scope) {
    absl::Nullable<Type*> resolved_callee_type = nullptr;
    if (_callee_type != nullptr) {
        auto callee_type = _callee_type->resolve(options, scope);
        FORWARD_ERROR(callee_type);

        resolved_callee_type = std::move(callee_type).value();
    }

    Scope::TypeList resolved_argument_types;
    resolved_argument_types.reserve(_argument_types.size());
    for (auto* argument : _argument_types) {
        auto argument_type = argument->resolve(options, scope);
        FORWARD_ERROR(argument_type);

        resolved_argument_types.push_back(std::move(argument_type).value());
    }

    absl::Nullable<Type*> resolved_return_type = nullptr;
    if (_return_type != nullptr) {
        auto return_type = _return_type->resolve(options, scope);
        FORWARD_ERROR(return_type);

        resolved_return_type = std::move(return_type).value();
    }

    return scope.get_resolved_function_type(resolved_callee_type, resolved_argument_types,
                                            resolved_return_type);
}

}  // namespace rain::lang::ast
