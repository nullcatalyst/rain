#pragma once

#include "absl/base/nullability.h"
#include "absl/strings/str_cat.h"
#include "llvm/ADT/SmallVector.h"
#include "rain/lang/ast/type/type.hpp"
#include "rain/util/result.hpp"

namespace rain::lang::ast {

using ArgumentTypeList = llvm::SmallVector<Type*, 4>;

class FunctionType : public Type {
    ArgumentTypeList      _argument_types;
    absl::Nullable<Type*> _return_type;

  public:
    FunctionType(ArgumentTypeList argument_types, absl::Nullable<Type*> return_type)
        : _argument_types(argument_types), _return_type(return_type) {}

    ~FunctionType() override = default;

    // Type
    [[nodiscard]] constexpr serial::TypeKind kind() const noexcept override {
        return serial::TypeKind::Function;
    }
    [[nodiscard]] std::string             name() const noexcept override;
    [[nodiscard]] constexpr lex::Location location() const noexcept override {
        // Function types do not have a location. They are implicitly defined.
        return lex::Location();
    }

    // FunctionType
    [[nodiscard]] constexpr const ArgumentTypeList& argument_types() const noexcept {
        return _argument_types;
    }

    [[nodiscard]] absl::Nullable<Type*> return_type() const noexcept { return _return_type; }

    [[nodiscard]] util::Result<absl::Nonnull<Type*>> resolve(Options& options,
                                                             Scope&   scope) override;
};

}  // namespace rain::lang::ast
