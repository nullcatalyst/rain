#pragma once

#include "cirrus/ast/type/type.hpp"
#include "cirrus/util/twine.hpp"

namespace cirrus::ast {

struct UnresolvedTypeData : public TypeData {
    util::Twine name;
};

struct UnresolvedType : public IType<UnresolvedType, TypeVtbl, UnresolvedTypeData> {
    using IType<UnresolvedType, TypeVtbl, UnresolvedTypeData>::IType;

    static const TypeVtbl _vtbl;

    [[nodiscard]] static bool is(const Type type) noexcept { return type.vtbl() == &_vtbl; }
    [[nodiscard]] static UnresolvedType alloc(util::Twine name) noexcept;

    [[nodiscard]] constexpr const util::Twine& name() const noexcept { return _data->name; }
};

}  // namespace cirrus::ast
