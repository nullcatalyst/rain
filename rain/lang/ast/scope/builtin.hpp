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
    absl::Nonnull<Type*> _bool_type;
    absl::Nonnull<Type*> _i32_type;
    absl::Nonnull<Type*> _i64_type;
    absl::Nonnull<Type*> _f32_type;
    absl::Nonnull<Type*> _f64_type;

  public:
    BuiltinScope();
    ~BuiltinScope() override = default;

    [[nodiscard]] absl::Nullable<Scope*>      parent() const noexcept override { return nullptr; }
    [[nodiscard]] absl::Nonnull<ModuleScope*> module() const noexcept override {
        util::console_error(
            "the builtin scope is not part of any module, and is instead shared between many "
            "modules");
        std::abort();
    }
    [[nodiscard]] absl::Nonnull<BuiltinScope*> builtin() const noexcept override {
        return const_cast<BuiltinScope*>(this);
    }

    [[nodiscard]] absl::Nonnull<Type*> bool_type() const noexcept { return _bool_type; }
    [[nodiscard]] absl::Nonnull<Type*> i32_type() const noexcept { return _i32_type; }
    [[nodiscard]] absl::Nonnull<Type*> i64_type() const noexcept { return _i64_type; }
    [[nodiscard]] absl::Nonnull<Type*> f32_type() const noexcept { return _f32_type; }
    [[nodiscard]] absl::Nonnull<Type*> f64_type() const noexcept { return _f64_type; }

    [[nodiscard]] absl::Nonnull<FunctionType*> get_function_type(
        const TypeList& argument_types, absl::Nullable<Type*> return_type) noexcept override;

    void add_type(const std::string_view name, std::unique_ptr<Type> type) noexcept override {
        util::console_error(
            "the builtin scope is immutable and cannot have custom types added to it");
        std::abort();
    }

    void add_method(absl::Nonnull<Type*> callee_type, const TypeList& argument_types,
                    const std::string_view            name,
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
