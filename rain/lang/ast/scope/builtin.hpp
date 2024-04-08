#pragma once

#include <cstdlib>
#include <memory>
#include <vector>

#include "absl/container/flat_hash_map.h"
#include "absl/container/flat_hash_set.h"
#include "rain/lang/ast/scope/scope.hpp"
#include "rain/lang/ast/type/type.hpp"
#include "rain/lang/ast/var/external_function.hpp"
#include "rain/util/console.hpp"

namespace rain::lang::ast {

class BuiltinScope : public Scope {
    // Primitive types. These are stored separately for easy access (so that they don't need to be
    // looked up by name for every literal).
    absl::Nonnull<Type*> _bool_type;
    absl::Nonnull<Type*> _i32_type;
    absl::Nonnull<Type*> _i64_type;
    absl::Nonnull<Type*> _f32_type;
    absl::Nonnull<Type*> _f64_type;
    absl::Nonnull<Type*> _f32x4_type;

    std::vector<std::unique_ptr<ExternalFunctionVariable>> _external_functions;

  public:
    BuiltinScope();
    ~BuiltinScope() override = default;

    [[nodiscard]] absl::Nullable<Scope*>      parent() const noexcept override { return nullptr; }
    [[nodiscard]] absl::Nonnull<ModuleScope*> module() const noexcept override {
        util::panic(
            "the builtin scope is not part of any module, and is instead shared between many "
            "modules");
    }
    [[nodiscard]] absl::Nonnull<BuiltinScope*> builtin() const noexcept override {
        return const_cast<BuiltinScope*>(this);
    }

    [[nodiscard]] absl::Nonnull<Type*> bool_type() const noexcept { return _bool_type; }
    [[nodiscard]] absl::Nonnull<Type*> i32_type() const noexcept { return _i32_type; }
    [[nodiscard]] absl::Nonnull<Type*> i64_type() const noexcept { return _i64_type; }
    [[nodiscard]] absl::Nonnull<Type*> f32_type() const noexcept { return _f32_type; }
    [[nodiscard]] absl::Nonnull<Type*> f64_type() const noexcept { return _f64_type; }
    [[nodiscard]] absl::Nonnull<Type*> f32x4_type() const noexcept { return _f32x4_type; }
    [[nodiscard]] const std::vector<std::unique_ptr<ExternalFunctionVariable>>& external_functions()
        const noexcept {
        return _external_functions;
    }

    ////////////////////////////////////////////////////////////////
    // Add AST nodes

    absl::Nonnull<Type*> add_named_type(const std::string_view name,
                                        std::unique_ptr<Type>  type) noexcept override;

    absl::Nonnull<FunctionVariable*> create_unresolved_function(
        const std::string_view name, absl::Nullable<FunctionType*> function_type,
        lex::Location location) noexcept override;

    absl::Nonnull<Variable*> add_variable(const std::string_view    name,
                                          std::unique_ptr<Variable> variable) noexcept override;

    void declare_external_function(std::unique_ptr<ExternalFunctionVariable> variable);

  private:
    absl::Nonnull<Type*> _add_builtin_type(const std::string_view name,
                                           std::unique_ptr<Type>  type) noexcept;
};

}  // namespace rain::lang::ast
