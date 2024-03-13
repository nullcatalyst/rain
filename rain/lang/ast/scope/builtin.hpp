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
    // Primitive types. These are stored separately for easy access (so that they don't need to be
    // looked up by name for every literal).
    Type* _bool_type;
    Type* _i32_type;
    Type* _i64_type;
    Type* _f32_type;
    Type* _f64_type;

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

    [[nodiscard]] FunctionType* get_function_type(
        const TypeList& argument_types, std::optional<Type*> return_type) noexcept override;

    void add_type(const std::string_view name, std::unique_ptr<Type> type) noexcept override {
        util::console_error(
            "the builtin scope is immutable and cannot have custom types added to it");
        std::abort();
    }

    void add_method(Type* callee_type, const std::string_view name,
                    std::unique_ptr<FunctionVariable> variable) noexcept override {
        util::console_error(
            "the builtin scope is immutable and cannot have custom methods added to it");
        std::abort();
    }

    void add_variable(const std::string_view    name,
                      std::unique_ptr<Variable> variable) noexcept override {
        util::console_error(
            "the builtin scope is immutable and cannot have custom variables added to it");
        std::abort();
    }
};

}  // namespace rain::lang::ast
