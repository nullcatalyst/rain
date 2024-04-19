#include "rain/lang/ast/expr/binary_operator.hpp"

#include <optional>
#include <string_view>

#include "absl/strings/str_cat.h"
#include "rain/lang/err/binary_operator.hpp"
#include "rain/lang/err/syntax.hpp"
#include "rain/util/assert.hpp"

namespace rain::lang::ast {

namespace {

std::optional<std::string_view> get_operator_method_name(serial::BinaryOperatorKind op) {
    switch (op) {
        case serial::BinaryOperatorKind::Add:
            return "__add__";
        case serial::BinaryOperatorKind::Subtract:
            return "__sub__";
        case serial::BinaryOperatorKind::Multiply:
            return "__mul__";
        case serial::BinaryOperatorKind::Divide:
            return "__div__";
        case serial::BinaryOperatorKind::Modulo:
            return "__rem__";
        case serial::BinaryOperatorKind::Equal:
            return "__eq__";
        case serial::BinaryOperatorKind::NotEqual:
            return "__ne__";
        case serial::BinaryOperatorKind::Less:
            return "__lt__";
        case serial::BinaryOperatorKind::LessEqual:
            return "__le__";
        case serial::BinaryOperatorKind::Greater:
            return "__gt__";
        case serial::BinaryOperatorKind::GreaterEqual:
            return "__ge__";
        case serial::BinaryOperatorKind::ArrayIndex:
            return "__getitem__";
        default:
            return std::nullopt;
    }
}

}  // namespace

bool BinaryOperatorExpression::compile_time_capable() const noexcept {
    return _lhs->compile_time_capable() && _rhs->compile_time_capable();
}

util::Result<void> BinaryOperatorExpression::validate(Options& options, Scope& scope) {
    {
        auto result = _lhs->validate(options, scope);
        FORWARD_ERROR(result);
    }
    {
        auto result = _rhs->validate(options, scope);
        FORWARD_ERROR(result);
    }

    if (_op == serial::BinaryOperatorKind::Assign) {
        if (!_lhs->is_assignable()) {
            return ERR_PTR(err::SyntaxError, _lhs->location(),
                           "left-hand side of assignment must be an assignable expression");
        }
        return {};
    }

    const auto method_name = get_operator_method_name(_op);
    IF_DEBUG {
        if (!method_name.has_value()) {
            return ERR_PTR(err::SyntaxError, _op_location,
                           "invalid binary operator; this is an internal error");
        }
    }

    {
        // First check if there is a method that takes self exactly.
        const Scope::TypeList argument_types{_lhs->type(), _rhs->type()};

        _method = scope.find_function(method_name.value(), _lhs->type(), argument_types);
        if (_method == nullptr) {
            return ERR_PTR(
                err::BinaryOperatorError, _lhs->location(), _rhs->location(), _op_location,
                absl::StrCat(
                    "no matching binary operator method found, looking for method named \"",
                    method_name.value(), "\" on type \"", _lhs->type()->display_name(), "\""));
        }
    }

    {
        // TODO: Check if the method can be called with a reference?
    }

    _type = _method->function_type()->return_type();
    return {};
}

}  // namespace rain::lang::ast
