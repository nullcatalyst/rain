#pragma once

#include <optional>
#include <vector>

#include "cirrus/ast/type/type.hpp"
#include "cirrus/util/string.hpp"

namespace cirrus::ast {

class TupleType : public Type {
    std::vector<TypePtr> _parts;

  public:
    TupleType(std::vector<TypePtr> parts) : _parts{std::move(parts)} {}

    [[nodiscard]] static std::shared_ptr<TupleType> alloc(std::vector<TupleType> parts) {
        return std::make_shared<TupleType>(std::move(parts));
    }

    [[nodiscard]] constexpr TypeKind kind() const noexcept override { return TypeKind::TupleType; }

    [[nodiscard]] constexpr const std::vector<TypePtr>& parts() const& noexcept { return _parts; }
    [[nodiscard]] constexpr std::vector<TypePtr>&& parts() && noexcept { return std::move(_parts); }
};

}  // namespace cirrus::ast
