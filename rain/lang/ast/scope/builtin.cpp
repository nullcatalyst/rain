#include "rain/lang/ast/scope/builtin.hpp"

#include "rain/lang/ast/type/opaque.hpp"
#include "rain/lang/ast/var/builtin_function.hpp"

namespace rain::lang::ast {

BuiltinScope::BuiltinScope() {
    _bool_type = std::make_shared<OpaqueType>();
    _i32_type  = std::make_shared<OpaqueType>();
    _i64_type  = std::make_shared<OpaqueType>();
    _f32_type  = std::make_shared<OpaqueType>();
    _f64_type  = std::make_shared<OpaqueType>();

    _owned_types.emplace(_bool_type);
    _owned_types.emplace(_i32_type);
    _owned_types.emplace(_i64_type);
    _owned_types.emplace(_f32_type);
    _owned_types.emplace(_f64_type);

    _named_types.emplace("bool", _bool_type.get());
    _named_types.emplace("i32", _i32_type.get());
    _named_types.emplace("i64", _i64_type.get());
    _named_types.emplace("f32", _f32_type.get());
    _named_types.emplace("f64", _f64_type.get());

    {
        auto i32_binop_type =
            get_function_type({_i32_type.get(), _i32_type.get()}, _i32_type.get());

        _methods.emplace(std::make_tuple(_i32_type.get(), "__add__"),
                         make_builtin_function_variable(
                             "__add__", i32_binop_type, [](auto& builder, auto& arguments) {
                                 return builder.CreateAdd(arguments[0], arguments[1]);
                             }));
        _methods.emplace(std::make_tuple(_i32_type.get(), "__sub__"),
                         make_builtin_function_variable(
                             "__sub__", i32_binop_type, [](auto& builder, auto& arguments) {
                                 return builder.CreateSub(arguments[0], arguments[1]);
                             }));
        _methods.emplace(std::make_tuple(_i32_type.get(), "__mul__"),
                         make_builtin_function_variable(
                             "__mul__", i32_binop_type, [](auto& builder, auto& arguments) {
                                 return builder.CreateMul(arguments[0], arguments[1]);
                             }));
        _methods.emplace(std::make_tuple(_i32_type.get(), "__div__"),
                         make_builtin_function_variable(
                             "__div__", i32_binop_type, [](auto& builder, auto& arguments) {
                                 return builder.CreateSDiv(arguments[0], arguments[1]);
                             }));
        _methods.emplace(std::make_tuple(_i32_type.get(), "__rem__"),
                         make_builtin_function_variable(
                             "__rem__", i32_binop_type, [](auto& builder, auto& arguments) {
                                 return builder.CreateSRem(arguments[0], arguments[1]);
                             }));
    }
}

FunctionTypePtr BuiltinScope::get_function_type(const TypeList& argument_types,
                                                Type*           return_type) noexcept {
#if !defined(NDEBUG)
    // All of the argument types must be non-null.
    // The return type MAY BE null (in the case of a void function).
    // All of the involved types (arguments and return type) must be owned by this scope.
    for (const auto* argument_type : argument_types) {
        assert(argument_type != nullptr);
        assert(_owned_types.contains(argument_type));
    }
    if (return_type != nullptr) {
        assert(_owned_types.contains(return_type));
    }
#endif  // !defined(NDEBUG)

    if (auto it = _function_types.find(std::make_tuple(argument_types, return_type));
        it != _function_types.end()) {
        return it->second;
    }

    auto function_type = std::make_shared<FunctionType>(argument_types, return_type);
    _function_types.emplace(std::make_tuple(argument_types, return_type), function_type);
    return function_type;
}

std::optional<TypePtr> BuiltinScope::find_type(const std::string_view name) const noexcept {
    if (const auto it = _named_types.find(name); it != _named_types.end()) {
        return it->second;
    }
    return std::nullopt;
}

std::optional<VariablePtr> BuiltinScope::find_method(const TypePtr&         callee_type,
                                                     const std::string_view name) const noexcept {
    if (const auto it = _methods.find(std::make_tuple(callee_type.get(), name));
        it != _methods.end()) {
        return it->second;
    }
    return std::nullopt;
}

std::optional<VariablePtr> BuiltinScope::find_variable(const std::string_view name) const noexcept {
    if (const auto it = _variables.find(name); it != _variables.end()) {
        return it->second;
    }
    return std::nullopt;
}

}  // namespace rain::lang::ast
