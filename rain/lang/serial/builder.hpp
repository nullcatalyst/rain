#pragma once

#include <vector>

#include "rain/lang/serial/expression.hpp"
#include "rain/lang/serial/module.hpp"
#include "rain/lang/serial/type.hpp"
#include "rain/lang/serial/variable.hpp"

namespace rain::lang::serial {

class Builder {
    std::vector<Type>       _types;
    std::vector<Expression> _expressions;
    std::vector<Variable>   _variables;
    std::vector<uint32_t>   _indices;
    std::vector<char>       _strings;

  public:
    Builder() = default;

    Builder(const Builder&)            = delete;
    Builder& operator=(const Builder&) = delete;

    Builder(Builder&&)            = default;
    Builder& operator=(Builder&&) = default;

    ~Builder() = default;

    uint32_t push_type(const Type& type) {
        const auto index = _types.size();
        _types.push_back(type);
        return index;
    }

    uint32_t push_expression(const Expression& expression) {
        const auto index = _expressions.size();
        _expressions.push_back(expression);
        return index;
    }

    uint32_t push_variable(const Variable& variable) {
        const auto index = _variables.size();
        _variables.push_back(variable);
        return index;
    }

    uint32_t push_index(const uint32_t index) {
        const auto result = _indices.size();
        _indices.push_back(index);
        return result;
    }

    uint32_t addString(const std::string_view string) {
        const auto start = _strings.size();
        _strings.insert(_strings.end(), string.begin(), string.end());
        const auto end = _strings.size();
        _indices.push_back(start);
        _indices.push_back(end);
        return start;
    }

    Module build();
};

}  // namespace rain::lang::serial
