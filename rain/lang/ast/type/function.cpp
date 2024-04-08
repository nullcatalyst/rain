#include "rain/lang/ast/type/function.hpp"

#include "rain/lang/ast/scope/scope.hpp"

namespace rain::lang::ast {

FunctionType::FunctionType(ArgumentTypeList argument_types, absl::Nullable<Type*> return_type)
    : _argument_types(argument_types), _return_type(return_type) {}

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

    if (_return_type != nullptr) {
        return absl::StrCat("fn(", argument_type_names, ") -> ", _return_type->display_name());
    } else {
        return absl::StrCat("fn(", argument_type_names, ")");
    }
}

util::Result<absl::Nonnull<Type*>> FunctionType::resolve(Options& options, Scope& scope) {
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

    return scope.get_function_type(resolved_argument_types, resolved_return_type);
}

}  // namespace rain::lang::ast
