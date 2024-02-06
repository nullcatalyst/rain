#pragma once

#include <string_view>

#include "cirrus/ast/type/type.hpp"

namespace cirrus::ast {

struct OpaqueTypeData : public TypeData {
    std::string_view name;
};

struct OpaqueType : public IType<OpaqueType, TypeVtbl, OpaqueTypeData> {
    using IType<OpaqueType, TypeVtbl, OpaqueTypeData>::IType;

    static const TypeVtbl _vtbl;

    [[nodiscard]] static bool       is(const Type type) noexcept { return type.vtbl() == &_vtbl; }
    [[nodiscard]] static OpaqueType alloc(std::string_view name) noexcept;

    [[nodiscard]] constexpr std::string_view name() const noexcept { return _data->name; }
};

}  // namespace cirrus::ast
