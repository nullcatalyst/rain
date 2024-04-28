#pragma once

#include <optional>
#include <string_view>

#include "rain/lang/serial/kind.hpp"

namespace rain::lang::serial {

std::optional<std::string_view>  get_unary_operator_name(const UnaryOperatorKind op);
std::optional<UnaryOperatorKind> get_unary_operator_kind(const std::string_view op_name);

std::optional<std::string_view>   get_binary_operator_name(const BinaryOperatorKind op);
std::optional<BinaryOperatorKind> get_binary_operator_kind(const std::string_view op_name);

namespace OperatorNames {

// MARK: Unary Operators

// Arithmetic
constexpr const std::string_view Negative = "__neg__";
constexpr const std::string_view Positive = "__pos__";

// Bitwise
constexpr const std::string_view Not = "__not__";

// Other
constexpr const std::string_view Reference = "__ref__";
constexpr const std::string_view HasValue  = "__has__";

// MARK: Binary Operators

// Assignment
constexpr const std::string_view Assign = "__assign__";

// Arithmetic
constexpr const std::string_view Add       = "__add__";
constexpr const std::string_view Subtract  = "__sub__";
constexpr const std::string_view Multiply  = "__mul__";
constexpr const std::string_view Divide    = "__div__";
constexpr const std::string_view Remainder = "__rem__";

// Comparison Operators
constexpr const std::string_view Equal        = "__eq__";
constexpr const std::string_view NotEqual     = "__ne__";
constexpr const std::string_view Less         = "__lt__";
constexpr const std::string_view LessEqual    = "__le__";
constexpr const std::string_view Greater      = "__gt__";
constexpr const std::string_view GreaterEqual = "__ge__";

// Bitwise
constexpr const std::string_view And         = "__and__";
constexpr const std::string_view Or          = "__or__";
constexpr const std::string_view Xor         = "__xor__";
constexpr const std::string_view ShiftLeft   = "__shl__";
constexpr const std::string_view ShiftRight  = "__shr__";
constexpr const std::string_view RotateLeft  = "__rotl__";
constexpr const std::string_view RotateRight = "__rotr__";

// Conversion
constexpr const std::string_view CastFrom   = "__from__";
constexpr const std::string_view ArrayIndex = "__get__";

}  // namespace OperatorNames

}  // namespace rain::lang::serial
