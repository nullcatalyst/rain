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
    std::string_view     name;
    absl::Nonnull<Type*> type;
};

class StructType : public Type {
    /** The name of the struct. */
    std::string_view _name;

    /** The set of fields contained in the struct. */
    std::vector<StructField> _fields;

    lex::Location _location;

  public:
    StructType(std::string_view name, std::vector<StructField> fields, lex::Location location);

    // Type
    [[nodiscard]] constexpr serial::TypeKind kind() const noexcept override {
        return serial::TypeKind::Struct;
    }
    [[nodiscard]] std::string display_name() const noexcept override { return std::string(_name); }
    [[nodiscard]] constexpr lex::Location location() const noexcept override { return _location; }

    // StructType
    [[nodiscard]] std::string_view name() const noexcept { return _name; }

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

    [[nodiscard]] util::Result<absl::Nonnull<Type*>> resolve(Options& options,
                                                             Scope&   scope) override;
};

}  // namespace rain::lang::ast
