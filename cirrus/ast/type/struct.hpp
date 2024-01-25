#pragma once

#include <optional>
#include <string_view>
#include <vector>

#include "cirrus/ast/type/type.hpp"

namespace cirrus::ast {

struct StructTypeFieldData {
    std::string_view name;
    Type             type;
};

struct StructTypeData : public TypeData {
    std::optional<std::string_view>  name;
    std::vector<StructTypeFieldData> fields;
};

struct StructType : public IType<StructType, TypeVtbl, StructTypeData> {
    using IType<StructType, TypeVtbl, StructTypeData>::IType;

    static const TypeVtbl _vtbl;

    [[nodiscard]] static bool       is(const Type type) noexcept { return type.vtbl() == &_vtbl; }
    [[nodiscard]] static StructType alloc(std::vector<StructTypeFieldData> fields) noexcept;
    [[nodiscard]] static StructType alloc(std::string_view                 name,
                                          std::vector<StructTypeFieldData> fields) noexcept;
    [[nodiscard]] static StructType alloc(std::optional<std::string_view>  name,
                                          std::vector<StructTypeFieldData> fields) noexcept;

    [[nodiscard]] constexpr bool is_named() const noexcept { return _data->name != std::nullopt; }
    [[nodiscard]] constexpr std::optional<std::string_view> name() const noexcept {
        return _data->name;
    }
    [[nodiscard]] constexpr std::string_view name_or_empty() const noexcept {
        return _data->name.value_or("");
    }
    [[nodiscard]] constexpr const std::vector<StructTypeFieldData>& fields() const noexcept {
        return _data->fields;
    }
};

}  // namespace cirrus::ast
