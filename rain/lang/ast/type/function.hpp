#pragma once

#include "absl/base/nullability.h"
#include "absl/strings/str_cat.h"
#include "llvm/ADT/SmallVector.h"
#include "rain/lang/ast/type/type.hpp"
#include "rain/util/result.hpp"

namespace rain::lang::ast {

class FunctionType : public Type {
    llvm::SmallVector<Type*, 4> _argument_types;
    absl::Nullable<Type*>       _return_type;

  public:
    FunctionType(llvm::SmallVector<Type*, 4> argument_types, absl::Nullable<Type*> return_type)
        : _argument_types(argument_types), _return_type(return_type) {}

    ~FunctionType() override = default;

    [[nodiscard]] constexpr serial::TypeKind kind() const noexcept override {
        return serial::TypeKind::Function;
    }
    [[nodiscard]] std::string                                  name() const noexcept override;
    [[nodiscard]] constexpr const llvm::SmallVector<Type*, 4>& argument_types() const noexcept {
        return _argument_types;
    }

    [[nodiscard]] absl::Nullable<Type*> return_type() const noexcept { return _return_type; }

    [[nodiscard]] util::Result<absl::Nonnull<Type*>> resolve(Scope& scope) override;
};

}  // namespace rain::lang::ast
