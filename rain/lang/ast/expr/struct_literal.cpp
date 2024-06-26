#include "rain/lang/ast/expr/struct_literal.hpp"

#include <optional>
#include <string_view>

#include "absl/strings/str_cat.h"
#include "rain/lang/err/simple.hpp"

namespace rain::lang::ast {

bool StructLiteralExpression::is_compile_time_capable() const noexcept {
    for (const auto& field : _fields) {
        if (!field.value->is_compile_time_capable()) {
            return false;
        }
    }

    return true;
}

util::Result<void> StructLiteralExpression::validate(Options& options, Scope& scope) {
    auto type = _type->resolve(options, scope);
    FORWARD_ERROR(type);
    _type = std::move(type).value();

    // Check if the type is a struct.
    if (_type->kind() != serial::TypeKind::Struct) {
        return ERR_PTR(err::SimpleError,
                       absl::StrCat("type \"", _type->display_name(), "\" is not a struct type"));
    }

    StructType* struct_type = static_cast<StructType*>(_type);

    // Validate each field.
    for (auto& field : _fields) {
        // Check if the type has a field with the given name.
        for (int i = 0, end = struct_type->fields().size(); i < end; ++i) {
            if (struct_type->fields()[i].name == field.name) {
                field.index = i;
                break;
            }
        }
        if (field.index == -1) {
            return ERR_PTR(err::SimpleError,
                           absl::StrCat("struct type \"", struct_type->display_name(),
                                        "\" has no field named \"", field.name, "\""));
        }

        auto result = field.value->validate(options, scope);
        FORWARD_ERROR(result);
    }

    return {};
}

}  // namespace rain::lang::ast
