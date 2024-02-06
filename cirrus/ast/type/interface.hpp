#pragma once

#include <optional>
#include <string_view>
#include <vector>

#include "cirrus/ast/type/type.hpp"

namespace cirrus::ast {

struct InterfaceTypeMethodData {
    std::string_view name;
    Type             type;
};

struct InterfaceTypeData : public TypeData {
    std::string_view                     name;
    std::vector<InterfaceTypeMethodData> fields;
};

struct InterfaceType : public IType<InterfaceType, TypeVtbl, InterfaceTypeData> {
    using IType<InterfaceType, TypeVtbl, InterfaceTypeData>::IType;

    static const TypeVtbl _vtbl;

    [[nodiscard]] static bool is(const Type type) noexcept { return type.vtbl() == &_vtbl; }
    [[nodiscard]] static InterfaceType alloc(std::string_view                     name,
                                             std::vector<InterfaceTypeMethodData> fields) noexcept;
};

}  // namespace cirrus::ast
