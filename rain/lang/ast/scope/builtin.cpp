#include "rain/lang/ast/scope/builtin.hpp"

#include <array>
#include <vector>

#include "rain/lang/ast/type/meta.hpp"
#include "rain/lang/ast/type/opaque.hpp"
#include "rain/lang/ast/type/struct.hpp"
#include "rain/lang/ast/var/block.hpp"
#include "rain/lang/ast/var/builtin_function.hpp"
#include "rain/lang/ast/var/function.hpp"
#include "rain/lang/code/context.hpp"
#include "rain/lang/serial/operator_names.hpp"

namespace rain::lang::ast {

BuiltinScope::BuiltinScope() {
    _bool_type = _add_builtin_type("bool", std::make_unique<OpaqueType>("bool"));
    _i32_type  = _add_builtin_type("i32", std::make_unique<OpaqueType>("i32"));
    _i64_type  = _add_builtin_type("i64", std::make_unique<OpaqueType>("i64"));
    _f32_type  = _add_builtin_type("f32", std::make_unique<OpaqueType>("f32"));
    _f64_type  = _add_builtin_type("f64", std::make_unique<OpaqueType>("f64"));
    _f32x4_type =
        _add_builtin_type("f32x4", std::make_unique<StructType>(
                                       "f32x4",
                                       [_f32_type = this->_f32_type]() -> std::vector<StructField> {
                                           // std::initializer_list of move-only types is not
                                           // allowed (for some unknown reason).
                                           std::vector<StructField> fields;
                                           fields.reserve(4);
                                           fields.emplace_back(StructField{"x", _f32_type});
                                           fields.emplace_back(StructField{"y", _f32_type});
                                           fields.emplace_back(StructField{"z", _f32_type});
                                           fields.emplace_back(StructField{"w", _f32_type});
                                           return fields;
                                       }(),
                                       lex::Location()));

    {
        // The no-return-value, no-argument function type has to be specially added to the builtin
        // scope, as without having any types to determine which scope should own the type, no scope
        // would be selected as its owner. Intentionally adding it here avoids that issue.
        const TypeList no_args;
        auto           function_type = std::make_unique<FunctionType>(nullptr, no_args, nullptr);
        auto*          function_type_ptr = function_type.get();
        _function_types.insert_or_assign(std::make_tuple(nullptr, no_args, nullptr),
                                         function_type_ptr);
        _owned_types.insert(std::move(function_type));
    }

#include "rain/lang/ast/scope/builtin/all.inl"
}

absl::Nonnull<Type*> BuiltinScope::add_named_type(const std::string_view name,
                                                  std::unique_ptr<Type>  type) noexcept {
    util::panic("the builtin scope is immutable and cannot have custom types added to it");
}

absl::Nonnull<FunctionVariable*> BuiltinScope::create_unresolved_function(
    const std::string_view name, absl::Nullable<FunctionType*> function_type,
    lex::Location location) noexcept {
    util::panic("the builtin scope is immutable and cannot have custom functions added to it");
}

absl::Nonnull<Variable*> BuiltinScope::add_variable(std::unique_ptr<Variable> variable) noexcept {
    util::panic("the builtin scope is immutable and cannot have custom variables added to it");
}

absl::Nonnull<FunctionVariable*> BuiltinScope::add_resolved_function(
    std::unique_ptr<FunctionVariable> function_variable) noexcept {
    util::panic("the builtin scope is immutable and cannot have custom functions added to it");
}

void BuiltinScope::declare_external_function(std::unique_ptr<ExternalFunctionVariable> variable) {
    _function_variables.insert_or_assign(
        std::make_tuple(variable->name(), nullptr, variable->function_type()->argument_types()),
        variable.get());
    _external_functions.emplace_back(std::move(variable));
}

absl::Nonnull<Type*> BuiltinScope::_add_builtin_type(const std::string_view name,
                                                     std::unique_ptr<Type>  type) noexcept {
    auto* const type_ptr = type.get();
    _named_types.emplace(name, type_ptr);
    _owned_types.insert(std::move(type));

    auto  meta_type     = std::make_unique<MetaType>(type_ptr);
    auto* meta_type_ptr = meta_type.get();
    _meta_types.insert_or_assign(type_ptr, meta_type.get());
    _owned_types.insert(std::move(meta_type));

    auto variable = std::make_unique<BlockVariable>(name, meta_type_ptr, false);
    _named_variables.insert_or_assign(name, variable.get());
    _owned_variables.insert(std::move(variable));

    return type_ptr;
}

}  // namespace rain::lang::ast
