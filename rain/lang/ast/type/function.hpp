#pragma once

#include "llvm/ADT/SmallVector.h"
#include "rain/lang/ast/type/type.hpp"

namespace rain::lang::ast {

class FunctionType : public Type {
    llvm::SmallVector<Type*, 4> _parameter_types;
    Type*                       _return_type;

  public:
    FunctionType(llvm::SmallVector<Type*, 4> parameter_types, Type* return_type)
        : _parameter_types(parameter_types), _return_type(return_type) {}

    ~FunctionType() override = default;

    [[nodiscard]] constexpr const llvm::SmallVector<Type*, 4>& parameter_types() const noexcept {
        return _parameter_types;
    }

    [[nodiscard]] Type* return_type() const noexcept { return _return_type; }
};

using FunctionTypePtr = std::shared_ptr<FunctionType>;

}  // namespace rain::lang::ast
