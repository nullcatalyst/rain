#include "rain/lang/ast/expr/slice_literal.hpp"

#include <optional>
#include <string_view>

#include "absl/strings/str_cat.h"
#include "rain/lang/err/simple.hpp"

namespace rain::lang::ast {

SliceLiteralExpression::SliceLiteralExpression(absl::Nonnull<Type*>                     type,
                                               std::vector<std::unique_ptr<Expression>> elements,
                                               lex::Location                            location)
    : _type(type), _elements(std::move(elements)), _location(location) {}

bool SliceLiteralExpression::is_compile_time_capable() const noexcept {
    for (const auto& element : _elements) {
        if (!element->is_compile_time_capable()) {
            return false;
        }
    }

    return true;
}

bool SliceLiteralExpression::is_constant() const noexcept {
    for (const auto& element : _elements) {
        if (!element->is_constant()) {
            return false;
        }
    }

    return true;
}

util::Result<void> SliceLiteralExpression::validate(Options& options, Scope& scope) {
    IF_DEBUG {
        // Check if the type is a slice.
        // This should never happen.
        if (_type->kind() != serial::TypeKind::Slice) {
            return ERR_PTR(err::SimpleError,
                           absl::StrCat("type \"", _type->display_name(),
                                        "\" is not an slice type; this is an internal error"));
        }
    }

    {
        auto result = _type->resolve(options, scope);
        FORWARD_ERROR(result);

        _type = std::move(result).value();
    }

    SliceType& slice_type = static_cast<SliceType&>(*_type);

    // Validate each field.
    for (int i = 0, end = _elements.size(); i < end; ++i) {
        auto element = _elements[i].get();

        auto result = element->validate(options, scope);
        FORWARD_ERROR(result);

        // Check if the element type matches the array type.
        if (element->type() != &slice_type.type()) {
            return ERR_PTR(
                err::SimpleError,
                absl::StrCat("element type \"", element->type()->display_name(), "\" at index ", i,
                             " does not match slice's element type \"",
                             slice_type.type().display_name(), "\""));
        }
    }

    return {};
}

}  // namespace rain::lang::ast
