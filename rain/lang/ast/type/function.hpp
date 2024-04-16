#pragma once

#include "absl/base/nullability.h"
#include "absl/strings/str_cat.h"
#include "llvm/ADT/SmallVector.h"
#include "rain/lang/ast/type/type.hpp"
#include "rain/util/result.hpp"

namespace rain::lang::ast {

using ArgumentTypeList = llvm::SmallVector<Type*, 4>;

class FunctionType : public Type {
    absl::Nullable<Type*> _callee_type;
    ArgumentTypeList      _argument_types;
    absl::Nullable<Type*> _return_type;
    bool                  _self_argument = false;

  public:
    FunctionType(absl::Nullable<Type*> callee_type, ArgumentTypeList argument_types,
                 absl::Nullable<Type*> return_type, bool self_argument = false);
    ~FunctionType() override = default;

    // Type
    [[nodiscard]] constexpr serial::TypeKind kind() const noexcept override {
        return serial::TypeKind::Function;
    }
    [[nodiscard]] std::string             display_name() const noexcept override;
    [[nodiscard]] constexpr lex::Location location() const noexcept override {
        // Function types do not have a location. They are implicitly defined.
        return lex::Location();
    }

    // FunctionType
    [[nodiscard]] constexpr auto        callee_type() const noexcept { return _callee_type; }
    [[nodiscard]] constexpr const auto& argument_types() const noexcept { return _argument_types; }
    [[nodiscard]] constexpr auto        return_type() const noexcept { return _return_type; }
    [[nodiscard]] constexpr bool has_self_argument() const noexcept { return _self_argument; }

    [[nodiscard]] util::Result<absl::Nonnull<Type*>> resolve(Options& options,
                                                             Scope&   scope) override;
};

}  // namespace rain::lang::ast
