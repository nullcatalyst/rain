#include "rain/lang/serial/operator_names.hpp"

#include <algorithm>
#include <array>
#include <optional>
#include <string_view>
#include <tuple>

namespace rain::lang::serial {

// MARK: Unary Operators

std::optional<std::string_view> get_unary_operator_name(UnaryOperatorKind op) {
    static constexpr const std::array<std::string_view, 5> unary_operators{
        // Arithmetic
        OperatorNames::Negative,
        OperatorNames::Positive,
        // Bitwise
        OperatorNames::Not,
        // Other
        OperatorNames::Reference,
        OperatorNames::HasValue,
    };
    // Subtract 1 because we don't store a name for the Unknown operator (which is always 0).
    static_assert(unary_operators.size() == static_cast<int>(UnaryOperatorKind::Count) - 1);

    int index = static_cast<int>(op);
    if (index <= 0 || index > unary_operators.size()) [[unlikely]] {
        return std::nullopt;
    }

    return unary_operators[index - 1];
}

std::optional<UnaryOperatorKind> get_unary_operator_kind(const std::string_view op_name) {
    static constexpr const std::array<std::tuple<std::string_view, UnaryOperatorKind>, 5>
        unary_operators{
    // This NEEDS to be sorted according to the string values of the operator names.
    // The sorting is used to allow for a binary search to find the operator name.
#define UNARY_OPERATOR(name) std::tuple{OperatorNames::name, UnaryOperatorKind::name}
            // clang-format off
            /* __has__ */ UNARY_OPERATOR(HasValue),
            /* __neg__ */ UNARY_OPERATOR(Negative),
            /* __not__ */ UNARY_OPERATOR(Not),
            /* __pos__ */ UNARY_OPERATOR(Positive),
            /* __ref__ */ UNARY_OPERATOR(Reference),
    // clang-format on
#undef UNARY_OPERATOR
        };

    const auto it =
        std::lower_bound(unary_operators.begin(), unary_operators.end(), op_name,
                         [](const auto& lhs, const auto& rhs) { return std::get<0>(lhs) < rhs; });
    if (it != unary_operators.end() && std::get<0>(*it) == op_name) {
        return std::get<1>(*it);
    }
    return std::nullopt;
}

// MARK: Binary Operators

std::optional<std::string_view> get_binary_operator_name(BinaryOperatorKind op) {
    static constexpr const std::array<std::string_view, 21> binary_operators{
        // Assignment
        OperatorNames::Assign,
        // Arithmetic
        OperatorNames::Add,
        OperatorNames::Subtract,
        OperatorNames::Multiply,
        OperatorNames::Divide,
        OperatorNames::Remainder,
        // Comparison
        OperatorNames::Equal,
        OperatorNames::NotEqual,
        OperatorNames::Less,
        OperatorNames::LessEqual,
        OperatorNames::Greater,
        OperatorNames::GreaterEqual,
        // Bitwise
        OperatorNames::And,
        OperatorNames::Or,
        OperatorNames::Xor,
        OperatorNames::ShiftLeft,
        OperatorNames::ShiftRight,
        OperatorNames::RotateLeft,
        OperatorNames::RotateRight,
        // Conversion
        OperatorNames::CastFrom,
        OperatorNames::ArrayIndex,
    };
    // Subtract 1 because we don't store a name for the Unknown operator (which is always 0).
    static_assert(binary_operators.size() == static_cast<int>(BinaryOperatorKind::Count) - 1);

    int index = static_cast<int>(op);
    if (index <= 0 || index > binary_operators.size()) [[unlikely]] {
        return std::nullopt;
    }

    return binary_operators[index - 1];
}

std::optional<BinaryOperatorKind> get_binary_operator_kind(const std::string_view op_name) {
    static constexpr const std::array<std::tuple<std::string_view, BinaryOperatorKind>, 21>
        binary_operators{
    // This NEEDS to be sorted according to the string values of the operator names.
    // The sorting is used to allow for a binary search to find the operator name.
#define BINARY_OPERATOR(name) std::tuple{OperatorNames::name, BinaryOperatorKind::name}
            // clang-format off
            /* __add__ */ BINARY_OPERATOR(Add),
            /* __and__ */ BINARY_OPERATOR(And),
            /* __assign__ */ BINARY_OPERATOR(Assign),
            /* __div__ */ BINARY_OPERATOR(Divide),
            /* __eq__ */ BINARY_OPERATOR(Equal),
            /* __from__ */ BINARY_OPERATOR(CastFrom),
            /* __ge__ */ BINARY_OPERATOR(GreaterEqual),
            /* __get__ */ BINARY_OPERATOR(ArrayIndex),
            /* __gt__ */ BINARY_OPERATOR(Greater),
            /* __le__ */ BINARY_OPERATOR(LessEqual),
            /* __lt__ */ BINARY_OPERATOR(Less),
            /* __mul__ */ BINARY_OPERATOR(Multiply),
            /* __ne__ */ BINARY_OPERATOR(NotEqual),
            /* __or__ */ BINARY_OPERATOR(Or),
            /* __rem__ */ BINARY_OPERATOR(Remainder),
            /* __rotl__ */ BINARY_OPERATOR(RotateLeft),
            /* __rotr__ */ BINARY_OPERATOR(RotateRight),
            /* __shl__ */ BINARY_OPERATOR(ShiftLeft),
            /* __shr__ */ BINARY_OPERATOR(ShiftRight),
            /* __sub__ */ BINARY_OPERATOR(Subtract),
            /* __xor__ */ BINARY_OPERATOR(Xor),
    // clang-format on
#undef BINARY_OPERATOR
        };
    // Subtract 1 because we don't store a name for the Unknown operator (which is always 0).
    static_assert(binary_operators.size() == static_cast<int>(BinaryOperatorKind::Count) - 1);

    const auto it =
        std::lower_bound(binary_operators.begin(), binary_operators.end(), op_name,
                         [](const auto& lhs, const auto& rhs) { return std::get<0>(lhs) < rhs; });
    if (it != binary_operators.end() && std::get<0>(*it) == op_name) {
        return std::get<1>(*it);
    }
    return std::nullopt;
}

}  // namespace rain::lang::serial
