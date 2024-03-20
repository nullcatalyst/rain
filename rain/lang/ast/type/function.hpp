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

#if !defined(NDEBUG)
    [[nodiscard]] std::string debug_name() const noexcept override {
        std::string argument_type_names;
        for (size_t i = 0; i < _argument_types.size(); ++i) {
            if (i > 0) {
                argument_type_names =
                    absl::StrCat(", ", argument_type_names, _argument_types[i]->debug_name());
            } else {
                argument_type_names = _argument_types[i]->debug_name();
            }
        }

        if (_return_type != nullptr) {
            return absl::StrCat("fn(", argument_type_names, ") -> ", _return_type->debug_name());
        } else {
            return absl::StrCat("fn(", argument_type_names, ")");
        }
    }
#endif  // !defined(NDEBUG)

    [[nodiscard]] constexpr serial::TypeKind kind() const noexcept override {
        return serial::TypeKind::Function;
    }
    [[nodiscard]] constexpr const llvm::SmallVector<Type*, 4>& argument_types() const noexcept {
        return _argument_types;
    }

    [[nodiscard]] absl::Nullable<Type*> return_type() const noexcept { return _return_type; }

    [[nodiscard]] util::Result<absl::Nonnull<Type*>> resolve(Scope& scope) override;
};

}  // namespace rain::lang::ast
