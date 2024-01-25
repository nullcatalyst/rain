#pragma once

#include <optional>
#include <string_view>
#include <vector>

#include "cirrus/ast/type/type.hpp"

namespace cirrus::ast {

struct UnresolvedTypeData : public TypeData {
    std::string_view name;
};

struct UnresolvedType : public IType<UnresolvedType, TypeVtbl, UnresolvedTypeData> {
    using IType<UnresolvedType, TypeVtbl, UnresolvedTypeData>::IType;

    static const TypeVtbl _vtbl;

    [[nodiscard]] static bool is(const Type type) noexcept { return type.vtbl() == &_vtbl; }
    [[nodiscard]] static UnresolvedType alloc(std::string_view name) noexcept;

    [[nodiscard]] constexpr std::string_view name() const noexcept { return _data->name; }
};

}  // namespace cirrus::ast
