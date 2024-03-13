#pragma once

#include <array>
#include <cstdint>
#include <memory>
#include <optional>
#include <string_view>
#include <tuple>

#include "rain/serial/expression.hpp"
#include "rain/serial/type.hpp"
#include "rain/serial/variable.hpp"
#include "rain/util/result.hpp"

namespace rain::serial {

class Builder;

class Module {
    /**
     * The list of types.
     *
     * This is a contiguous array of types, whose memory is backed by the `_memory` block.
     */
    const Type* _types       = nullptr;
    size_t      _types_count = 0;

    /**
     * The list of expressions.
     *
     * This is a contiguous array of expressions, whose memory is backed by the `_memory` block.
     */
    const Expression* _expressions       = nullptr;
    size_t            _expressions_count = 0;

    /**
     * The list of variables.
     *
     * This is a contiguous array of variables, whose memory is backed by the `_memory` block.
     */
    const Variable* _variables       = nullptr;
    size_t          _variables_count = 0;

    /**
     * The list of indices used by various lists.
     *
     * This is a contiguous array of variables, whose memory is backed by the `_memory` block.
     */
    const uint32_t* _indices       = nullptr;
    size_t          _indices_count = 0;

    /**
     * The list of strings used in the module.
     *
     * This is a contiguous array of strings, whose memory is part of the `_memory` block.
     */
    const char* _strings      = nullptr;
    size_t      _strings_size = 0;

    std::unique_ptr<uint8_t[]> _memory;
    size_t                     _memory_size;

    friend class Builder;

  public:
    Module() = default;

    Module(const Module&)            = delete;
    Module& operator=(const Module&) = delete;

    Module(Module&&)            = default;
    Module& operator=(Module&&) = default;

    ~Module() = default;

    [[nodiscard]] static util::Result<Module> from_memory(std::unique_ptr<uint8_t[]> data,
                                                          const size_t               size);

    std::tuple<std::unique_ptr<uint8_t[]>, uint32_t> release() noexcept {
        _types             = nullptr;
        _types_count       = 0;
        _expressions       = nullptr;
        _expressions_count = 0;
        _variables         = nullptr;
        _variables_count   = 0;
        _indices           = nullptr;
        _indices_count     = 0;
        _strings           = nullptr;
        _strings_size      = 0;

        const auto size = _memory_size;
        _memory_size    = 0;

        return std::make_tuple(std::move(_memory), size);
    }

    [[nodiscard]] constexpr std::optional<Type> type(const uint32_t id) const noexcept {
        if (id >= _types_count) {
            return std::nullopt;
        }
        return _types[id];
    }

    [[nodiscard]] constexpr std::optional<Expression> expression(const uint32_t id) const noexcept {
        if (id >= _expressions_count) {
            return std::nullopt;
        }
        return _expressions[id];
    }

    [[nodiscard]] constexpr std::optional<Variable> variable(const uint32_t id) const noexcept {
        if (id >= _variables_count) {
            return std::nullopt;
        }
        return _variables[id];
    }

    [[nodiscard]] constexpr std::optional<uint32_t> index(const uint32_t id) const noexcept {
        if (id >= _indices_count) {
            return std::nullopt;
        }
        return _indices[id];
    }

    [[nodiscard]] constexpr std::optional<std::string_view> string(
        const uint32_t start, const uint32_t end) const noexcept {
        if (start >= end || end >= _strings_size) {
            return std::nullopt;
        }
        return std::string_view(&_strings[start], &_strings[end]);
    }

  private:
    /**
     * Look at the header of the memory block, and initialize the pointers to the various lists.
     *
     * This method does no bounds checking. It is a helper method to reduce code duplication.
     * It is the responsibility of the caller to ensure that the data is valid.
     */
    void _set_pointers();
};

}  // namespace rain::serial
