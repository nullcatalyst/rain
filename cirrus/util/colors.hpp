#pragma once

#include <string_view>

#if !defined(CIRRUS_OUTPUT_COLORS)
#if defined(__wasm__)
#define CIRRUS_OUTPUT_COLORS 0
#else
#define CIRRUS_OUTPUT_COLORS 1
#endif
#endif  // !defined(CIRRUS_OUTPUT_COLORS)

#if CIRRUS_OUTPUT_COLORS

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

#endif  // CIRRUS_OUTPUT_COLORS

// Example usage: std::cout << COUT_COLOR_RED("Hello, " << name << "!") << std::endl;
#define COUT_COLOR_BOLD(text) ANSI_BOLD << text << ANSI_RESET
#define COUT_COLOR_RED(text) ANSI_RED << text << ANSI_RESET
#define COUT_COLOR_GREEN(text) ANSI_GREEN << text << ANSI_RESET
#define COUT_COLOR_BLUE(text) ANSI_BLUE << text << ANSI_RESET
#define COUT_COLOR_CYAN(text) ANSI_CYAN << text << ANSI_RESET
