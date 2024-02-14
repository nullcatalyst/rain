#pragma once

#include <optional>
#include <vector>

#include "cirrus/ast/type/type.hpp"
#include "cirrus/util/string.hpp"

namespace cirrus::ast {

struct StructTypeFieldData {
    util::String name;
    TypePtr      type;
};

class StructType : public Type {
    std::optional<util::String>      _name;
    std::vector<StructTypeFieldData> _fields;

  public:
    StructType(std::optional<util::String> name, std::vector<StructTypeFieldData> fields)
        : _name{std::move(name)}, _fields{std::move(fields)} {}

    [[nodiscard]] static std::shared_ptr<StructType> alloc(
        std::optional<util::String> name, std::vector<StructTypeFieldData> fields) {
        return std::make_shared<StructType>(std::move(name), std::move(fields));
    }

    [[nodiscard]] NodeKind kind() const noexcept override { return NodeKind::StructType; }

    [[nodiscard]] constexpr bool is_named() const noexcept { return _name.has_value(); }
    [[nodiscard]] util::String   name_or_empty() const noexcept {
        return _name.value_or(util::String{});
    }
    [[nodiscard]] constexpr const std::vector<StructTypeFieldData>& fields() const& noexcept {
        return _fields;
    }
    [[nodiscard]] constexpr std::vector<StructTypeFieldData>&& fields() && noexcept {
        return std::move(_fields);
    }
};

}  // namespace cirrus::ast
