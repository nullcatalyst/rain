#include "rain/lang/ast/scope/builtin.hpp"

#include <vector>

#include "rain/lang/ast/type/opaque.hpp"
#include "rain/lang/ast/type/struct.hpp"
#include "rain/lang/ast/var/builtin_function.hpp"
#include "rain/lang/ast/var/function.hpp"

namespace rain::lang::ast {

BuiltinScope::BuiltinScope() {
    _bool_type  = _add_owned_named_type("bool", std::make_unique<OpaqueType>("bool"));
    _i32_type   = _add_owned_named_type("i32", std::make_unique<OpaqueType>("i32"));
    _i64_type   = _add_owned_named_type("i64", std::make_unique<OpaqueType>("i64"));
    _f32_type   = _add_owned_named_type("f32", std::make_unique<OpaqueType>("f32"));
    _f64_type   = _add_owned_named_type("f64", std::make_unique<OpaqueType>("f64"));
    _f32x4_type = _add_owned_named_type(
        "f32x4",
        std::make_unique<StructType>("f32x4", std::vector<StructField>{
                                                  StructField{.name = "x", .type = _f32_type},
                                                  StructField{.name = "y", .type = _f32_type},
                                                  StructField{.name = "z", .type = _f32_type},
                                                  StructField{.name = "w", .type = _f32_type},
                                              }));

#include "rain/lang/ast/scope/builtin_methods.inl"
}

absl::Nonnull<FunctionType*> BuiltinScope::get_function_type(
    const TypeList& argument_types, absl::Nullable<Type*> return_type) noexcept {
    IF_DEBUG {
        // All of the argument types must be non-null.
        // The return type MAY BE null (in the case of a void function).
        // All of the involved types (arguments and return type) must be owned by this scope.
        for ([[maybe_unused]] const auto* argument_type : argument_types) {
            assert(argument_type != nullptr);
            assert(_owned_types.contains(argument_type));
        }
        if (return_type != nullptr) {
            if (!_owned_types.contains(return_type)) {
                util::console_error("builtin scope does not own return type");
                std::abort();
            }
        }
    }

    auto type = Scope::_get_function_type(argument_types, return_type);
    if (type == nullptr) {
        util::console_error(
            "failed to find function type: cannot create a function type with types not found in "
            "scope at any level (this should never happen)");
        std::abort();
    }
    return type;
}

void BuiltinScope::declare_external_function(std::unique_ptr<ExternalFunctionVariable> variable) {
    _function_variables.insert_or_assign(
        std::make_tuple(nullptr, variable->function_type()->argument_types(), variable->name()),
        variable.get());
    _external_functions.emplace_back(std::move(variable));
}

}  // namespace rain::lang::ast
