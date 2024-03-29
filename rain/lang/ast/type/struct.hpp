#pragma once

#include <optional>
#include <string_view>

#include "absl/base/nullability.h"
#include "rain/lang/ast/expr/expression.hpp"
#include "rain/lang/ast/type/type.hpp"
#include "rain/lang/serial/expression.hpp"
#include "rain/util/maybe_owned_ptr.hpp"

namespace rain::lang::ast {

struct StructField {
    std::string_view          name;
    util::MaybeOwnedPtr<Type> type;
};

class StructType : public Type {
    /** The name of the struct, if it has one. */
    std::optional<std::string_view> _name;

    /** The set of fields contained in the struct. */
    std::vector<StructField> _fields;

  public:
    StructType(std::optional<std::string_view> name, std::vector<StructField> fields)
        : _name(std::move(name)), _fields(std::move(fields)) {}

#if !defined(NDEBUG)
    [[nodiscard]] std::string debug_name() const noexcept override {
        if (_name.has_value()) {
            return "<struct:" + std::string(_name.value()) + ">";
        }
        return "<unnnamed struct>";
    }
#endif  // !defined(NDEBUG)

    [[nodiscard]] constexpr serial::TypeKind kind() const noexcept override {
        return serial::TypeKind::Struct;
    }
    [[nodiscard]] constexpr bool   is_named() const noexcept { return _name.has_value(); }
    [[nodiscard]] std::string_view name_or_empty() const noexcept {
        return _name.value_or(std::string_view());
    }

    [[nodiscard]] constexpr bool has_fields() const noexcept { return !_fields.empty(); }
    [[nodiscard]] constexpr const std::vector<StructField>& fields() const noexcept {
        return _fields;
    }

    [[nodiscard]] std::optional<int> find_member(std::string_view name) const noexcept {
        for (int i = 0; i < _fields.size(); ++i) {
            if (_fields[i].name == name) {
                return i;
            }
        }
        return std::nullopt;
    }

    [[nodiscard]] util::Result<absl::Nonnull<Type*>> resolve(Scope& scope) override;
};

}  // namespace rain::lang::ast
