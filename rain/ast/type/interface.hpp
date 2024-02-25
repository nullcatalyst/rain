#pragma once

#include <optional>
#include <string_view>
#include <vector>

#include "rain/ast/type/type.hpp"

namespace rain::ast {

struct InterfaceTypeMethodData {
    util::String name;
    TypePtr      type;
};

struct InterfaceType : public Type {
    util::String                         _name;
    std::vector<InterfaceTypeMethodData> _fields;

  public:
    InterfaceType(util::String name, std::vector<InterfaceTypeMethodData> fields)
        : _name{std::move(name)}, _fields{std::move(fields)} {}

    [[nodiscard]] static std::shared_ptr<InterfaceType> alloc(
        util::String name, std::vector<InterfaceTypeMethodData> fields) {
        return std::make_shared<InterfaceType>(std::move(name), std::move(fields));
    }

    [[nodiscard]] constexpr TypeKind kind() const noexcept override {
        return TypeKind::InterfaceType;
    }

    [[nodiscard]] util::String name() const noexcept { return _name; }
    [[nodiscard]] const std::vector<InterfaceTypeMethodData>& fields() const& noexcept {
        return _fields;
    }
    [[nodiscard]] std::vector<InterfaceTypeMethodData>&& fields() && noexcept {
        return std::move(_fields);
    }
};

}  // namespace rain::ast
