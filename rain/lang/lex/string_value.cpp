#include "rain/lang/lex/string_value.hpp"

#include <iostream>

namespace rain::lang::lex {

std::string string_value(std::string_view string) {
    std::string result;
    result.reserve(string.size());

    if (string.size() >= 2 && string.front() == '"' && string.back() == '"') {
        string = string.substr(1, string.size() - 2);
    }

    for (int i = 0, end = string.size(); i < end; ++i) {
        const auto c = string[i];
        if (c != '\\') {
            result.push_back(c);
            continue;
        }

        if (i + 1 < end) {
            switch (string[i + 1]) {
                case 'n':
                    result.push_back('\n');
                    break;
                case 'r':
                    result.push_back('\r');
                    break;
                case 't':
                    result.push_back('\t');
                    break;
                case '\\':
                    result.push_back('\\');
                    break;
                case '"':
                    result.push_back('"');
                    break;
                default:
                    result.push_back(string[i + 1]);
                    break;
            }
            ++i;
        }
    }

    return result;
}

}  // namespace rain::lang::lex
