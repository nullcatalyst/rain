#pragma once

#include <string_view>

#include "gtest/gtest.h"
#include "rain/lang/lexer.hpp"
#include "rain/lang/parser.hpp"

namespace {

testing::AssertionResult test_compile(const std::string_view code) {
    rain::lang::Lexer  lexer(code);
    rain::lang::Parser parser;
    auto               result = parser.parse(lexer);
    if (result.has_value()) {
        return testing::AssertionSuccess();
    }
    return testing::AssertionFailure() << result.error()->message();
}

}  // namespace
