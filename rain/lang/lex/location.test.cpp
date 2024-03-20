#include "rain/lang/lex/location.hpp"

#include <string_view>

#include "gtest/gtest.h"

using namespace rain;
using namespace rain::lang;
using namespace rain::lang::lex;

TEST(Location, find_start_of_line) {
    const std::string_view code = "line 1\nline 2\nline 3";
    const Location         location(code, code.data(), &code.data()[code.size()], 1, 1);

    {
        // Simple case.
        const auto start = location.find_start_of_line(&code.data()[11]);

        EXPECT_EQ(start, &code.data()[7]);
        EXPECT_EQ(std::string_view{start}, "line 2\nline 3");
    }

    {
        // On the first line.
        const auto start = location.find_start_of_line(&code.data()[2]);

        EXPECT_EQ(start, &code.data()[0]);
        EXPECT_EQ(std::string_view{start}, "line 1\nline 2\nline 3");
    }

    {
        // At the start of the file.
        const auto start = location.find_start_of_line(code.data());

        EXPECT_EQ(start, code.data());
        EXPECT_EQ(std::string_view{start}, "line 1\nline 2\nline 3");
    }
}

TEST(Location, find_end_of_line) {
    const std::string_view code = "line 1\nline 2\nline 3";
    const Location         location(code, code.data(), &code.data()[code.size()], 1, 1);

    {
        // Simple case.
        const auto end = location.find_end_of_line(&code.data()[11]);

        EXPECT_EQ(end, &code.data()[13]);
        EXPECT_EQ(std::string_view{end}, "\nline 3");
    }

    {
        // On the last line.
        const auto end = location.find_end_of_line(&code.data()[17]);

        EXPECT_EQ(end, &code.data()[code.size()]);
        EXPECT_EQ(std::string_view{end}, "");
    }

    {
        // At the start of the file.
        const auto end = location.find_end_of_line(&code.data()[code.size()]);

        EXPECT_EQ(end, &code.data()[code.size()]);
        EXPECT_EQ(std::string_view{end}, "");
    }
}

TEST(Location, previous_line) {
    const std::string_view code = "line 1\nline 2\nline 3";

    {
        // Simple case.
        const Location location(code, &code.data()[7], &code.data()[13], 2, 1);  // "line 2"
        const auto     previous_line = location.previous_line();

        EXPECT_EQ(previous_line.line, 1);
        EXPECT_EQ(previous_line.column, 1);
        EXPECT_EQ(previous_line.text(), "line 1");
    }

    {
        // On the first line.
        const Location location(code, code.data(), &code.data()[6], 1, 1);  // "line 1"
        const auto     previous_line = location.previous_line();

        EXPECT_EQ(previous_line.line, 1);
        EXPECT_EQ(previous_line.column, 1);
        EXPECT_EQ(previous_line.text(), "");
    }

    {
        // After the last line (eg: empty file, or missing token at the end of the file).
        const Location location(code, &code.data()[code.size()], &code.data()[code.size()], 3, 7);
        const auto     previous_line = location.previous_line();

        EXPECT_EQ(previous_line.line, 2);
        EXPECT_EQ(previous_line.column, 1);
        EXPECT_EQ(previous_line.text(), "line 2");
    }

    {
        // After the last line, but the file ends with a newline.
        const std::string_view code = "line 1\nline 2\nline 3\n";
        const Location location(code, &code.data()[code.size()], &code.data()[code.size()], 4, 1);
        const auto     previous_line = location.previous_line();

        EXPECT_EQ(previous_line.line, 3);
        EXPECT_EQ(previous_line.column, 1);
        EXPECT_EQ(previous_line.text(), "line 3");
    }
}

TEST(Location, next_line) {
    const std::string_view code = "line 1\nline 2\nline 3";

    {
        // Simple case.
        const Location location(code, &code.data()[7], &code.data()[13], 2, 1);  // "line 2"
        const auto     next_line = location.next_line();

        EXPECT_EQ(next_line.line, 3);
        EXPECT_EQ(next_line.column, 1);
        EXPECT_EQ(next_line.text(), "line 3");
    }

    {
        // On the last line.
        const Location location(code, &code.data()[14], &code.data()[code.size()], 3,
                                1);  // "line 3"
        const auto     next_line = location.next_line();

        EXPECT_EQ(next_line.line, 3);
        EXPECT_EQ(next_line.column, 7);
        EXPECT_EQ(next_line.text(), "");
    }

    {
        // After the last line (eg: empty file, or missing token at the end of the file).
        const Location location(code, &code.data()[code.size()], &code.data()[code.size()], 3, 7);
        const auto     next_line = location.next_line();

        EXPECT_EQ(next_line.line, 3);
        EXPECT_EQ(next_line.column, 7);
        EXPECT_EQ(next_line.text(), "");
    }

    {
        // After the last line, but the file ends with a newline.
        const std::string_view code = "line 1\nline 2\nline 3\n";
        const Location location(code, code.data() + code.size(), &code.data()[code.size()], 4, 1);
        const auto     next_line = location.next_line();

        EXPECT_EQ(next_line.line, 4);
        EXPECT_EQ(next_line.column, 1);
        EXPECT_EQ(next_line.text(), "");
    }
}
