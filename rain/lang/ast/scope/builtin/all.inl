#define ADD_BUILTIN_METHOD(name, callee_type, function_type, argument_types, ...)                \
    do {                                                                                         \
        auto method = make_builtin_function_variable(                                            \
            name, function_type, [](auto& ctx, auto& arguments) { __VA_ARGS__ });                \
        _function_variables.insert_or_assign(std::make_tuple(name, callee_type, argument_types), \
                                             method.get());                                      \
        _owned_variables.insert(std::move(method));                                              \
    } while (0)

// In order to use the OperatorNames namespace.
using namespace ::rain::lang::serial;

#include "rain/lang/ast/scope/builtin/f32_math.inl"
#include "rain/lang/ast/scope/builtin/f32x4_math.inl"
// #include "rain/lang/ast/scope/builtin/f32x4_shuffle.inl"
#include "rain/lang/ast/scope/builtin/f64_math.inl"
#include "rain/lang/ast/scope/builtin/i32_math.inl"
#include "rain/lang/ast/scope/builtin/i64_math.inl"

#undef ADD_BUILTIN_METHOD
