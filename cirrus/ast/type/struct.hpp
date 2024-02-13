#pragma once

#include <optional>
#include <string_view>
#include <vector>

#include "cirrus/ast/type/type.hpp"
#include "cirrus/util/twine.hpp"

namespace cirrus::ast {

struct StructTypeFieldData {
    util::Twine name;
    Type        type;
};

struct StructTypeData : public TypeData {
    std::optional<util::Twine>       name;
    std::vector<StructTypeFieldData> fields;
};

struct StructType : public IType<StructType, TypeVtbl, StructTypeData> {
    using IType<StructType, TypeVtbl, StructTypeData>::IType;

    static const TypeVtbl _vtbl;

    [[nodiscard]] static bool is(const Type type) noexcept { return type.vtbl() == &_vtbl; }

    [[nodiscard]] static StructType alloc(std::optional<util::Twine>       name,
                                          std::vector<StructTypeFieldData> fields) noexcept;

    [[nodiscard]] constexpr bool is_named() const noexcept { return _data->name != std::nullopt; }
    [[nodiscard]] constexpr const std::optional<util::Twine>& name() const noexcept {
        return _data->name;
    }
    [[nodiscard]] util::Twine name_or_empty() const noexcept {
        return _data->name.value_or(util::Twine{});
    }
    [[nodiscard]] constexpr const std::vector<StructTypeFieldData>& fields() const noexcept {
        return _data->fields;
    }
};

}  // namespace cirrus::ast
