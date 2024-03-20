#include "rain/lang/ast/scope/builtin.hpp"

#include "rain/lang/ast/type/opaque.hpp"
#include "rain/lang/ast/var/builtin_function.hpp"
//
// #include <cassert>

namespace rain::lang::ast {

BuiltinScope::BuiltinScope() {
    _bool_type = _owned_types.emplace(std::make_unique<OpaqueType>("bool")).first->get();
    _i32_type  = _owned_types.emplace(std::make_unique<OpaqueType>("i32")).first->get();
    _i64_type  = _owned_types.emplace(std::make_unique<OpaqueType>("i64")).first->get();
    _f32_type  = _owned_types.emplace(std::make_unique<OpaqueType>("f32")).first->get();
    _f64_type  = _owned_types.emplace(std::make_unique<OpaqueType>("f64")).first->get();

    _named_types.emplace("bool", _bool_type);
    _named_types.emplace("i32", _i32_type);
    _named_types.emplace("i64", _i64_type);
    _named_types.emplace("f32", _f32_type);
    _named_types.emplace("f64", _f64_type);

#define ADD_BUILTIN_METHOD(name, callee_type, function_type, argument_types, impl)                 \
    do {                                                                                           \
        auto method = make_builtin_function_variable(name, function_type,                          \
                                                     [](auto& builder, auto& arguments) { impl }); \
        _method_variables.emplace(std::make_tuple(callee_type, argument_types, name),              \
                                  method.get());                                                   \
        _owned_variables.emplace(std::move(method));                                               \
    } while (0)

    {
        auto  i32_binop_args = Scope::TypeList{_i32_type, _i32_type};
        auto* i32_binop_type = get_function_type(i32_binop_args, _i32_type);

        ADD_BUILTIN_METHOD("__add__", _i32_type, i32_binop_type, i32_binop_args,
                           { return builder.CreateAdd(arguments[0], arguments[1]); });
        ADD_BUILTIN_METHOD("__sub__", _i32_type, i32_binop_type, i32_binop_args,
                           { return builder.CreateSub(arguments[0], arguments[1]); });
        ADD_BUILTIN_METHOD("__mul__", _i32_type, i32_binop_type, i32_binop_args,
                           { return builder.CreateMul(arguments[0], arguments[1]); });
        ADD_BUILTIN_METHOD("__div__", _i32_type, i32_binop_type, i32_binop_args,
                           { return builder.CreateSDiv(arguments[0], arguments[1]); });
        ADD_BUILTIN_METHOD("__rem__", _i32_type, i32_binop_type, i32_binop_args,
                           { return builder.CreateSRem(arguments[0], arguments[1]); });

        auto  i32_as_f32_args = Scope::TypeList{_i32_type, _f32_type};
        auto* i32_as_f32_type = get_function_type(i32_as_f32_args, _f32_type);
        ADD_BUILTIN_METHOD("__as__", _i32_type, i32_as_f32_type, i32_as_f32_args, {
            return builder.CreateSIToFP(arguments[0], llvm::Type::getFloatTy(builder.getContext()));
        });
    }
}

absl::Nonnull<FunctionType*> BuiltinScope::get_function_type(
    const TypeList& argument_types, absl::Nullable<Type*> return_type) noexcept {
#if !defined(NDEBUG)
    // All of the argument types must be non-null.
    // The return type MAY BE null (in the case of a void function).
    // All of the involved types (arguments and return type) must be owned by this scope.
    for (const auto* argument_type : argument_types) {
        assert(argument_type != nullptr);
        assert(_owned_types.contains(argument_type));
    }
    if (return_type != nullptr) {
        if (!_owned_types.contains(return_type)) {
            util::console_error("builtin scope does not own return type");
            std::abort();
        }
    }
#endif  // !defined(NDEBUG)

    auto type = Scope::_get_function_type(argument_types, return_type);
    if (type == nullptr) {
        util::console_error(
            "failed to find function type: cannot create a function type with types not found in "
            "scope at any level (this should never happen)");
        std::abort();
    }
    return type;
}

}  // namespace rain::lang::ast
