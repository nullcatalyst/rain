#pragma once

#include <optional>
#include <vector>

#include "cirrus/ast/type/type.hpp"
#include "cirrus/util/string.hpp"

namespace cirrus::ast {

struct FunctionTypeArgumentData {
    util::String name;
    TypePtr      type;
};

class FunctionType : public Type {
    std::vector<FunctionTypeArgumentData> _arguments;
    std::optional<TypePtr>                _return_type;

  public:
    FunctionType(std::vector<FunctionTypeArgumentData> arguments,
                 std::optional<TypePtr>                return_type)
        : _arguments{std::move(arguments)}, _return_type{std::move(return_type)} {}

    [[nodiscard]] static std::shared_ptr<FunctionType> alloc(
        std::vector<FunctionTypeArgumentData> arguments, std::optional<TypePtr> return_type) {
        return std::make_shared<FunctionType>(std::move(arguments), std::move(return_type));
    }

    [[nodiscard]] NodeKind kind() const noexcept override { return NodeKind::FunctionType; }

    [[nodiscard]] const std::optional<TypePtr>& return_type() const noexcept {
        return _return_type;
    }
    [[nodiscard]] const std::vector<FunctionTypeArgumentData>& arguments() const& noexcept {
        return _arguments;
    }
    [[nodiscard]] std::vector<FunctionTypeArgumentData>&& arguments() && noexcept {
        return std::move(_arguments);
    }
};

}  // namespace cirrus::ast
