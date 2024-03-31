#pragma once

#include <array>
#include <span>
#include <string_view>
#include <tuple>

namespace {

static constexpr const std::array<std::string_view, 5> JS_MATH_IMPORTS{
    // clang-format off
    // <keep_sorted>
    "atan2",
    "cos",
    "sin",
    "sqrt",
    "tan",
    // </keep_sorted>
    // clang-format on
};

static constexpr const std::array<std::tuple<std::string_view, std::span<const std::string_view>>,
                                  1>
    JS_ALLOWED_NAMESPACES{
        // clang-format off
        // <keep_sorted>
        std::tuple{std::string_view{"math"}, std::span{JS_MATH_IMPORTS}},
        // </keep_sorted>
        // clang-format on
    };

}  // namespace
