#pragma once

#include <optional>
#include <vector>

#include "rain/ast/type/type.hpp"
#include "rain/util/string.hpp"

namespace rain::ast {

struct StructTypeFieldData {
    util::String name;
    TypePtr      type;
};

class StructType : public Type {
    std::optional<util::String>      _name;
    std::vector<StructTypeFieldData> _fields;

    lang::Location _struct_location;
    lang::Location _block_location;

  public:
    StructType(std::optional<util::String> name, std::vector<StructTypeFieldData> fields)
        : _name{std::move(name)}, _fields{std::move(fields)} {}
    StructType(std::optional<util::String> name, std::vector<StructTypeFieldData> fields,
               const lang::Location struct_location, const lang::Location block_location)
        : Type(struct_location.merge(block_location)),
          _name{std::move(name)},
          _fields{std::move(fields)},
          _struct_location(struct_location) {}

    [[nodiscard]] static std::shared_ptr<StructType> alloc(
        std::optional<util::String> name, std::vector<StructTypeFieldData> fields) {
        return std::make_shared<StructType>(std::move(name), std::move(fields));
    }
    [[nodiscard]] static std::shared_ptr<StructType> alloc(std::optional<util::String>      name,
                                                           std::vector<StructTypeFieldData> fields,
                                                           const lang::Location struct_location,
                                                           const lang::Location block_location) {
        return std::make_shared<StructType>(std::move(name), std::move(fields), struct_location,
                                            block_location);
    }

    [[nodiscard]] constexpr TypeKind kind() const noexcept override { return TypeKind::StructType; }

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

}  // namespace rain::ast
