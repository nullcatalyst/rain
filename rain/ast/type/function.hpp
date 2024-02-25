#pragma once

#include <optional>
#include <vector>

#include "rain/ast/type/type.hpp"
#include "rain/util/string.hpp"

namespace rain::ast {

class FunctionType : public Type {
    std::vector<TypePtr>   _argument_types;
    std::optional<TypePtr> _return_type;

  public:
    FunctionType(std::vector<TypePtr> argument_types, std::optional<TypePtr> return_type)
        : _argument_types{std::move(argument_types)}, _return_type{std::move(return_type)} {}

    [[nodiscard]] static std::shared_ptr<FunctionType> alloc(std::vector<TypePtr>   argument_types,
                                                             std::optional<TypePtr> return_type) {
        return std::make_shared<FunctionType>(std::move(argument_types), std::move(return_type));
    }

    [[nodiscard]] constexpr TypeKind kind() const noexcept override {
        return TypeKind::FunctionType;
    }

    [[nodiscard]] const std::optional<TypePtr>& return_type() const noexcept {
        return _return_type;
    }
    [[nodiscard]] const std::vector<TypePtr>& argument_types() const& noexcept {
        return _argument_types;
    }
    [[nodiscard]] std::vector<TypePtr>&& argument_types() && noexcept {
        return std::move(_argument_types);
    }
};

}  // namespace rain::ast
