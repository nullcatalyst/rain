#pragma once

#include <optional>
#include <string_view>
#include <vector>

#include "cirrus/ast/type/type.hpp"

namespace cirrus::ast {

struct FunctionTypeArgumentData {
    std::string_view name;
    Type             type;
};

struct FunctionTypeData : public TypeData {
    std::vector<FunctionTypeArgumentData> arguments;
    std::optional<Type>                   return_type;
};

struct FunctionType : public IType<FunctionType, TypeVtbl, FunctionTypeData> {
    using IType<FunctionType, TypeVtbl, FunctionTypeData>::IType;

    static const TypeVtbl _vtbl;

    [[nodiscard]] static bool         is(const Type type) noexcept { return type.vtbl() == &_vtbl; }
    [[nodiscard]] static FunctionType alloc(std::vector<FunctionTypeArgumentData> arguments,
                                            std::optional<Type> return_type) noexcept;

    [[nodiscard]] constexpr const std::optional<Type>& return_type() const noexcept {
        return _data->return_type;
    }
    [[nodiscard]] constexpr const std::vector<FunctionTypeArgumentData>& arguments()
        const noexcept {
        return _data->arguments;
    }
};

}  // namespace cirrus::ast
