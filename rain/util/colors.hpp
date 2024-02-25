#pragma once

#include <string_view>

#if !defined(RAIN_OUTPUT_COLORS)
#define RAIN_OUTPUT_COLORS 1
#endif  // !defined(RAIN_OUTPUT_COLORS)

#if RAIN_OUTPUT_COLORS

constexpr std::string_view ANSI_RESET = "\033[0m";
constexpr std::string_view ANSI_BOLD  = "\033[1m";
constexpr std::string_view ANSI_RED   = "\033[31m";
constexpr std::string_view ANSI_GREEN = "\033[32m";
constexpr std::string_view ANSI_BLUE  = "\033[34m";
constexpr std::string_view ANSI_CYAN  = "\033[36m";

#else

constexpr std::string_view ANSI_RESET = "";
constexpr std::string_view ANSI_BOLD  = "";
constexpr std::string_view ANSI_RED   = "";
constexpr std::string_view ANSI_GREEN = "";
constexpr std::string_view ANSI_BLUE  = "";
constexpr std::string_view ANSI_CYAN  = "";

#endif  // RAIN_OUTPUT_COLORS
