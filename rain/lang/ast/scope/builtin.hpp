#pragma once

#include <cstdlib>
#include <tuple>

#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "rain/lang/ast/scope/scope.hpp"
#include "rain/lang/ast/type/type.hpp"
#include "rain/util/log.hpp"

namespace rain::lang::ast {

class BuiltinScope : public Scope {
    absl::flat_hash_map<std::string_view, TypePtr>                        _named_types;
    absl::flat_hash_map<FunctionTypeKey, FunctionTypePtr>                 _function_types;
    absl::flat_hash_map<std::tuple<Type*, std::string_view>, VariablePtr> _methods;
    absl::flat_hash_map<std::string_view, VariablePtr>                    _variables;

    absl::flat_hash_set<TypePtr> _owned_types;

    // Primitive types. These are stored separately for easy access (so that they don't need to be
    // looked up by name for every literal).
    TypePtr _bool_type;
    TypePtr _i32_type;
    TypePtr _i64_type;
    TypePtr _f32_type;
    TypePtr _f64_type;

  public:
    BuiltinScope();
    ~BuiltinScope() override = default;

    [[nodiscard]] constexpr std::optional<Scope*> parent() const noexcept override {
        return std::nullopt;
    }

    [[nodiscard]] const ModuleScope& module() const noexcept override {
        util::console_error("the builtin scope is not part of a module");
        std::abort();
    }
    [[nodiscard]] ModuleScope& module() noexcept override {
        util::console_error("the builtin scope is not part of a module");
        std::abort();
    }

    [[nodiscard]] FunctionTypePtr get_function_type(const TypeList& argument_types,
                                                    Type*           return_type) noexcept override;

    [[nodiscard]] std::optional<TypePtr> find_type(
        const std::string_view name) const noexcept override;

    [[nodiscard]] std::optional<VariablePtr> find_method(
        const TypePtr& callee_type, const std::string_view name) const noexcept override;

    [[nodiscard]] std::optional<VariablePtr> find_variable(
        const std::string_view name) const noexcept override;

    void add_type(const std::string_view name, TypePtr type) noexcept override {
        util::console_error(
            "the builtin scope is immutable and cannot have custom types added to it");
        std::abort();
    }

    void add_method(const TypePtr& callee_type, const std::string_view name,
                    VariablePtr variable) noexcept override {
        util::console_error(
            "the builtin scope is immutable and cannot have custom methods added to it");
        std::abort();
    }

    void add_variable(const std::string_view name, VariablePtr variable) noexcept override {
        util::console_error(
            "the builtin scope is immutable and cannot have custom variables added to it");
        std::abort();
    }
};

}  // namespace rain::lang::ast
